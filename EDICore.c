//
//  EDICore.c
//  Crash'n Bob
//
//  Created by Taiki on 24/09/2015.
//  Copyright © 2015 Taiki. All rights reserved.
//

//Just for the record, EDI stand for Enhanced Driving Intelligence, nothing to see there

#include "main.h"

#define GET_CAR_NODE(section, posInLine, isLeft) section.node[posInLine][isLeft]
#define GET_CAR(section, goIn, posInLine, isLeft) (goIn ? section->goingIn : section->goingOut)[posInLine][isLeft]

void EDIRegisterCarInContext(CONTEXT context, CAR * newCar)
{
	//We check we simply have space
	if(context->nbCars == UINT_MAX)
	{
		printf("Too many cars!");
		crushCar(newCar);
		return;
	}
	
	//Okay, now, determine a slot on the external roads in which we could fit the new car
	byte baseSection = carGetRandomSectionDifferentOf(newCar->direction);
	for(byte i = 0; i < SECTION_EXTERNAL - 1; ++i)
	{
		if(EDIHaveFreeSlotToEnterSection(context, baseSection + i) && baseSection % SECTION_EXTERNAL != newCar->direction)
		{
			baseSection += i;
			baseSection %= SECTION_EXTERNAL;
			
			break;
		}
	}
	
	//Allocate memory for the car in our rendering array
	void * tmp = realloc(context->cars, (context->nbCars + 1) * sizeof(CAR *));
	if(tmp == NULL)
	{
		printf("Memory error, couldn't add car to array");
		crushCar(newCar);
		return;
	}
	
	//Insert the car in the main array
	context->cars = tmp;
	context->cars[context->nbCars++] = newCar;

	//Now, insert the car in the proper section so EDI can work her magic
	bool isLeft = context->EDI.externalRoads[baseSection].goingIn[0][0] != NULL;
	
	context->EDI.externalRoads[baseSection].goingIn[0][isLeft] = newCar;
	newCar->context.onLeftRoad = isLeft;
	newCar->context.section = baseSection;
}

void EDIRemoveCarFromContext(CONTEXT context, CAR * oldCar)
{
	for (uint i = 0, length = context->nbCars; i < length; ++i)
	{
		if(context->cars[i]->ID == oldCar->ID)
		{
#ifdef DEBUG_BUILD
			printf("\nFlushing car from array: current ptr %p (size %u)\n", context->cars, context->nbCars);
			printCar(oldCar);
#endif
			context->cars = (CAR **) removeItemAtIndexFromArray((void **) context->cars, context->nbCars--, i);
#ifdef DEBUG_BUILD
			printf("Resized: new ptr %p (size %u)\n\n", context->cars, context->nbCars);
#endif
			
			if(oldCar->context.section == SECTION_NODE)
				GET_CAR_NODE(context->EDI.node, oldCar->context.index, oldCar->context.onLeftRoad) = NULL;
			else
				GET_CAR((&context->EDI.externalRoads[oldCar->context.section]), false, oldCar->context.index, oldCar->context.onLeftRoad) = NULL;
			
			crushCar(oldCar);
			break;
		}
	}
}

static uint currentSession = 0;

void EDIProcessContext(CONTEXT context)
{
	//We first process car leaving the system
	for(byte section = 0; section < SECTION_EXTERNAL; ++section)
	{
		EDI_EXT_ROAD * workingSection = &context->EDI.externalRoads[section];
		
		for(uint pos = 0; pos < NB_SLOTS_BORDER; ++pos)
		{
			EDIProcessCarLeavingOnExternalRoad(context, workingSection, pos, true);
			EDIProcessCarLeavingOnExternalRoad(context, workingSection, pos, false);
		}
	}

	//We loop until we are sure all cars were processed
	currentSession = (currentSession + 1) % UINT_MAX;	//Prevent using UINT_MAX, we use the session not to evaluate two times the same car
	for(byte count = 0, carStuck = true; count < 2 && carStuck; ++count)
	{
		const uint forbiddenSlots[NB_EXTERNAL_SLOTS] = EXTERNAL_SLOTS;
		int8_t nextForbidden = NB_EXTERNAL_SLOTS - 1;
		carStuck = false;
		
		for(byte circle = 0; circle < 2; ++circle)
		{
			//Then the node so we hopefully have space for cars wishing to get in
			for(uint pos = NB_SLOTS_NODE; pos-- > 0;)
			{
				//We compute the internal road first
				if(circle == 0)
				{
					if(nextForbidden < 0 || forbiddenSlots[nextForbidden] != pos)
						carStuck |= EDIProcessCarInNode(context, pos, true, currentSession);
					else
						--nextForbidden;
				}
				else
					carStuck |= EDIProcessCarInNode(context, pos, false, currentSession);
			}
		}
	}
	
	//And, finally, cars trying to get in
	for(byte section = 0; section < SECTION_EXTERNAL; ++section)
	{
		EDI_EXT_ROAD * workingSection = &context->EDI.externalRoads[section];
		
		for(uint pos = NB_SLOTS_BORDER; pos-- > 0;)
		{
			EDIProcessCarEnteringOnExternalRoad(context, workingSection, pos, true);
			EDIProcessCarEnteringOnExternalRoad(context, workingSection, pos, false);
		}
	}
}

bool EDIProcessCarInNode(CONTEXT context, uint posInLine, bool isLeft, uint _currentSession)
{
	CAR * currentCar = GET_CAR_NODE(context->EDI.node, posInLine, isLeft);
	if(!EDICarShouldMove(currentCar, _currentSession))
		return false;
	else
		currentCar->context.session = _currentSession;
	
	//Okay, start processing
	const uint oldPosInLine = posInLine++;
	
	if(currentCar->context.onLeftRoad)
	{
		while(EDIIsSlotReservedForExternalRing(posInLine))
			++posInLine;
	}
	
	posInLine %= NB_SLOTS_NODE;
	
	const bool inLastQuarter = EDIIsCarInQuarterBeforeExit(currentCar->context.index, currentCar->direction), isInFrontOfExit = EDIIsCarInFrontOfExit(oldPosInLine & 0xff, currentCar->direction);
	bool wantToGoToLeft = !inLastQuarter, gotOut = false;
	const CAR backupCar = *currentCar;
	
	//So, in the node, there is couple of scenarios:
	//	1. We just entered, we try to get in the internal road (shorter, 4 less slots than the external ring)
	//	2. We try to get in the external road when getting in the last quarter to get out easily
	//	3. We are in front of the exit
	//		1. We are in the external road, we just get out
	//		2. We are still in the internal road, we try really hard to get out
	//		3. If the road is really too crowded, or the exit unavailable, ¯\_(ツ)_/¯, we make one more turn
	
	if(isInFrontOfExit && isLeft && EDIIsNodeSlotAvailableFullCheck(context->EDI.node, oldPosInLine, wantToGoToLeft))
	{
#ifdef DEBUG_BUILD
		printf("[%d]: Successfully steared toward the external ring\n", currentCar->ID);
#endif
	}
	
	else if(isInFrontOfExit && !isLeft && EDIIsExternalSlotAvailable(&context->EDI.externalRoads[currentCar->direction], false, NB_SLOTS_BORDER - 1, EDIIsCarOnLastStepExit(oldPosInLine & 0xff, currentCar->direction)))
	{
		//\o/
#ifdef DEBUG_BUILD
		printf("[%d]: Successfully got out of the ring\n", currentCar->ID);
#endif
		gotOut = true;
	}
	
	//Perfect case?
	else if(EDIIsNodeSlotAvailableFullCheck(context->EDI.node, posInLine, wantToGoToLeft))
	{
		//ᕕ( ᐛ )ᕗ
#ifdef DEBUG_BUILD
		printf("[%d]: Successfully went to our optimal goal\n", currentCar->ID);
#endif
	}
	
	//Can we go forward, even on the wrong line?
	else if(EDIIsNodeSlotAvailableFullCheck(context->EDI.node, posInLine, !wantToGoToLeft))
	{
		wantToGoToLeft = !wantToGoToLeft;
#ifdef DEBUG_BUILD
		printf("[%d]: Changing line forward\n", currentCar->ID);
#endif
	}

	//No space to go forward, but can we change line?
	else if(EDIIsNodeSlotAvailableFullCheck(context->EDI.node, oldPosInLine, wantToGoToLeft))
	{
		posInLine = oldPosInLine;
#ifdef DEBUG_BUILD
		printf("[%d]: Changing line\n", currentCar->ID);
#endif
	}

	//Nope :(
	else
	{
		return true;
	}
	
	if(gotOut)
	{
		currentCar->context.index = NB_SLOTS_BORDER - 1;
		currentCar->context.section = currentCar->direction;
		currentCar->context.onLeftRoad = EDIIsCarOnLastStepExit(oldPosInLine & 0xff, currentCar->direction);
		
		EDI_EXT_ROAD * section = &context->EDI.externalRoads[currentCar->direction];
		
		if(!EDITransitionCars(&GET_CAR_NODE(context->EDI.node, oldPosInLine, isLeft), &GET_CAR(section, false, currentCar->context.index, currentCar->context.onLeftRoad)))
			currentCar->context = backupCar.context;
		else
			context->rendering.sorted = false;
	}
	else
	{
		currentCar->context.index = posInLine & 0xff;
		currentCar->context.onLeftRoad = wantToGoToLeft;
		
		updateNodeData(currentCar);
		
		if(!EDITransitionCars(&GET_CAR_NODE(context->EDI.node, oldPosInLine, isLeft), &GET_CAR_NODE(context->EDI.node, currentCar->context.index, wantToGoToLeft)))
			currentCar->context = backupCar.context;
		else
			context->rendering.sorted = false;
	}
	
#ifdef DEBUG_BUILD
	printCar(currentCar);
#endif

	return false;
}

bool EDIIsCarInQuarterBeforeExit(uint16_t index, byte direction)
{
	byte divide = NB_SLOTS_NODE / 4;
	
	if((index - 4) / divide != index / divide)
		return false;

	switch (index / divide)
	{
		case 0:		return direction == DIR_WEST;
		case 1:		return direction == DIR_SOUTH;
		case 2:		return direction == DIR_EAST;
		case 3:		return direction == DIR_NORTH;
	}
	
	return false;
}

inline bool EDIIsCarInFrontOfExit(uint16_t index, byte direction)
{
	return ((byte [4]) EXIT_SLOTS)[direction] == index || EDIIsCarOnLastStepExit(index, direction);
}

inline bool EDIIsCarOnLastStepExit(uint16_t index, byte direction)
{
	return ((byte [4]) EXIT_SLOTS)[direction] == index - 1;
}

bool EDIIsNodeSlotAvailableFullCheck(EDI_NODE currentNode, uint posInLine, bool isLeft)
{
	if(isLeft)
	{
		for(byte forbiddenSpots[NB_EXTERNAL_SLOTS] = EXTERNAL_SLOTS, i = 0; i < sizeof(forbiddenSpots); ++i)
		{
			if(posInLine < forbiddenSpots[i])
				break;
			
			if(posInLine == forbiddenSpots[i])
				return false;
		}
	}
	
	return EDIIsNodeSlotAvailable(currentNode, posInLine, isLeft);
}

bool EDIIsNodeSlotAvailable(EDI_NODE currentNode, uint posInLine, bool isLeft)
{
	CAR * car = GET_CAR_NODE(currentNode, posInLine, isLeft);
	
	return car == NULL || car->status == STATUS_DANGER;
}

void EDIProcessCarLeavingOnExternalRoad(CONTEXT context, EDI_EXT_ROAD * workingSection, uint posInLine, bool isLeft)
{
	EDIProcessCarOnExternalRoad(context, workingSection, false, posInLine, isLeft);
}

void EDIProcessCarEnteringOnExternalRoad(CONTEXT context, EDI_EXT_ROAD * workingSection, uint posInLine, bool isLeft)
{
	EDIProcessCarOnExternalRoad(context, workingSection, true, posInLine, isLeft);
}

void EDIProcessCarOnExternalRoad(CONTEXT context, EDI_EXT_ROAD * workingSection, bool goingIn, uint posInLine, bool isLeft)
{
#ifdef DEBUG_BUILD
	const void * a = workingSection;
	const bool b = goingIn, d = isLeft;
	const uint c = posInLine;
#endif

	CAR * currentCar = GET_CAR(workingSection, goingIn, posInLine, isLeft);
	if(!EDICarShouldMove(currentCar, UINT_MAX))
		return;
	
#ifdef DEBUG_BUILD
	printf("Processing %p %d %u %d -> %p\n", a, b, c, d, currentCar);
#endif
	
	uint oldPosInLine = posInLine;
	const CAR backupCar = *currentCar;
	
	for(byte speed = currentCar->speed; speed > 0; --speed)
	{
		//The car is getting out \o/
		if(!goingIn && posInLine == 0)
		{
			EDIRemoveCarFromContext(context, currentCar);
			break;
		}
		
		else if(goingIn && posInLine == NB_SLOTS_BORDER - 1)
		{
			const byte entrySlots[4] = ENTRY_SLOTS;
			
			if(EDIIsNodeSlotAvailable(context->EDI.node, entrySlots[currentCar->context.section], isLeft))
			{
				currentCar->context.index = entrySlots[currentCar->context.section] - isLeft;
				currentCar->context.section = SECTION_NODE;
				currentCar->context.onLeftRoad = false;

				updateNodeData(currentCar);
				
#ifdef DEBUG_BUILD
				if(GET_CAR(workingSection, goingIn, oldPosInLine, isLeft) != currentCar)
				{
					printf("Trying to get a car to enter from an empty slot? WTF?!\n");
					break;
				}
				
				printf("[%d] Leaving the external road\n", currentCar->ID);
#endif

				if(!EDITransitionCars(&GET_CAR(workingSection, goingIn, oldPosInLine, isLeft), &GET_CAR_NODE(context->EDI.node, currentCar->context.index, isLeft)))
					currentCar->context = backupCar.context;
				else
					context->rendering.sorted = false;
				
#ifdef DEBUG_BUILD
				if(GET_CAR(workingSection, goingIn, oldPosInLine, isLeft) != NULL)
				{
					printf("Failed to move the car! WTFH? Reexecuting the line to tracing\n");
					if(!EDITransitionCars(&GET_CAR(workingSection, goingIn, oldPosInLine, isLeft), &GET_CAR_NODE(context->EDI.node, currentCar->context.index, isLeft)))
						currentCar->context = backupCar.context;
				}
#endif
			}
			
			break;
		}
		
		else
		{
			if(goingIn)
				++posInLine;
			else
				--posInLine;
			
			for(byte flag = 0; flag < 2; ++flag)
			{
				if(EDIIsExternalSlotAvailable(workingSection, goingIn, posInLine, flag))
				{
					if(EDITransitionCars(&GET_CAR(workingSection, goingIn, oldPosInLine, isLeft), &GET_CAR(workingSection, goingIn, posInLine, flag)))
					{
#ifdef DEBUG_BUILD
						printf("	Moved to %d %u %d %p %p\n", goingIn, posInLine, flag, GET_CAR(workingSection, goingIn, oldPosInLine, isLeft), GET_CAR(workingSection, goingIn, posInLine, flag));
#endif

						currentCar->context.onLeftRoad = flag;
						currentCar->context.index = posInLine & 0xffff;
						oldPosInLine = posInLine;
						isLeft = flag;
					}
					else
					{
						context->rendering.sorted = false;

#ifdef DEBUG_BUILD
						printf("	Failed at moving to %d %u %d %p %p?!\n", goingIn, posInLine, flag, GET_CAR(workingSection, goingIn, oldPosInLine, isLeft), GET_CAR(workingSection, goingIn, posInLine, flag));
#endif
					}

					break;
				}
			}
		}
	}
}

//This function is super time critical and should be as optimized as possible
bool EDIIsExternalSlotAvailable(EDI_EXT_ROAD * workingSection, bool goingIn, uint posInLine, bool isLeft)
{
	CAR * car = GET_CAR(workingSection, goingIn, posInLine, isLeft);
	
	return car == NULL || car->status == STATUS_DANGER;
}
