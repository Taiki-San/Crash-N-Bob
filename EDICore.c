//
//  EDICore.c
//  Crash'n Bob
//
//  Created by Taiki on 24/09/2015.
//  Copyright © 2015 Taiki. All rights reserved.
//

//Just for the record, EDI stand for Enhanced Driving Intelligence, nothing to see there

#include "main.h"

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
		if(context->cars[i] == oldCar)
		{
			context->cars = (CAR **) removeItemAtIndexFromArray((void **) context->cars, context->nbCars--, i);
			
			if(oldCar->context.section == SECTION_NODE)
				context->EDI.node.node[oldCar->context.index][oldCar->context.onLeftRoad] = NULL;
			else
				context->EDI.externalRoads[oldCar->context.section].goingIn[oldCar->context.index][oldCar->context.onLeftRoad] = NULL;
			break;
		}
	}
}

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
	for(byte count = 0, carStuck = true; count < 3 && carStuck; ++count)
	{
		const uint forbiddenSlots[4] = EXTERNAL_SLOTS;
		int8_t nextForbidden = 3;
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
						carStuck |= EDIProcessCarInNode(context, pos, true);
					else
						--nextForbidden;
				}
				else
					carStuck |= EDIProcessCarInNode(context, pos, false);
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
	
	finishedUpdateContext(context);
}

#pragma mark - Node processing

#define GET_CAR_NODE(section, posInLine, isLeft) section.node[posInLine][isLeft]
#define GET_CAR(section, goIn, posInLine, isLeft) (goIn ? section->goingIn : section->goingOut)[posInLine][isLeft]

bool EDIProcessCarInNode(CONTEXT context, uint posInLine, bool isLeft)
{
	CAR * currentCar = GET_CAR_NODE(context->EDI.node, posInLine, isLeft);
	if(currentCar == NULL)
		return false;
	
	//The car can't move for now
	else if(currentCar->status != STATUS_OK && currentCar->status != STATUS_DANGER)
		return false;
	
	//Okay, start processing
	const uint oldPosInLine = posInLine++;
	
	if(EDIIsSlotReservedForExternalRing(posInLine + 1))
		++posInLine;
	
	posInLine %= NB_SLOTS_NODE;
	
	const bool inLastQuarter = EDIIsCarInQuarterBeforeExit(currentCar->context.index, currentCar->direction), isInFrontOfExit = EDIIsCarInFrontOfExit(oldPosInLine & 0xff, currentCar->direction);
	bool wantToGoToLeft = !inLastQuarter, gotOut = false;
	
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
		puts("Successfully steared toward the external ring");
#endif
	}
	
	else if(isInFrontOfExit && !isLeft && EDIIsExternalSlotAvailable(&context->EDI.externalRoads[currentCar->direction], false, NB_SLOTS_BORDER - 1, EDIIsCarOnLastStepExit(oldPosInLine & 0xff, currentCar->direction)))
	{
		//\o/
#ifdef DEBUG_BUILD
		puts("Successfully got out of the ring");
#endif
		gotOut = true;
	}
	
	//Perfect case?
	else if(EDIIsNodeSlotAvailableFullCheck(context->EDI.node, posInLine, wantToGoToLeft))
	{
		//ᕕ( ᐛ )ᕗ
#ifdef DEBUG_BUILD
		puts("Successfully went to our optimal goal");
#endif
	}
	
	//Can we go forward, even on the wrong line?
	else if(EDIIsNodeSlotAvailableFullCheck(context->EDI.node, posInLine, !wantToGoToLeft))
	{
		wantToGoToLeft = !wantToGoToLeft;
	}

	//No space to go forward, but can we change line?
	else if(EDIIsNodeSlotAvailableFullCheck(context->EDI.node, oldPosInLine, wantToGoToLeft))
	{
		posInLine = oldPosInLine;
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
		
		GET_CAR_NODE(context->EDI.node, oldPosInLine, isLeft) = NULL;
		GET_CAR(section, false, currentCar->context.index, currentCar->context.onLeftRoad) = currentCar;
	}
	else
	{
		currentCar->context.index = posInLine & 0xff;
		currentCar->context.onLeftRoad = wantToGoToLeft;
		
		updateNodeData(currentCar);
		
		GET_CAR_NODE(context->EDI.node, oldPosInLine, isLeft) = NULL;
		GET_CAR_NODE(context->EDI.node, currentCar->context.index, wantToGoToLeft) = currentCar;
	}

	return false;
}

bool EDIIsCarInQuarterBeforeExit(byte index, byte direction)
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

inline bool EDIIsCarInFrontOfExit(byte index, byte direction)
{
	return ((byte [4]) EXIT_SLOTS)[direction] == index || EDIIsCarOnLastStepExit(index, direction);
}

inline bool EDIIsCarOnLastStepExit(byte index, byte direction)
{
	return ((byte [4]) EXIT_SLOTS)[direction] == index - 1;
}

bool EDIIsNodeSlotAvailableFullCheck(EDI_NODE currentNode, uint posInLine, bool isLeft)
{
	if(isLeft)
	{
		for(byte forbiddenSpots[4] = EXTERNAL_SLOTS, i = 0; i < 4; ++i)
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
	return GET_CAR_NODE(currentNode, posInLine, isLeft) == NULL;
}

#pragma mark - External road processing

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
	CAR * currentCar = GET_CAR(workingSection, goingIn, posInLine, isLeft);
	if(currentCar == NULL)
		return;
	
	uint oldPosInLine = posInLine;
	
	//The car can't move for now
	if(currentCar->status != STATUS_OK && currentCar->status != STATUS_DANGER)
		return;
	
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

				GET_CAR(workingSection, goingIn, oldPosInLine, isLeft) = NULL;
				GET_CAR_NODE(context->EDI.node, currentCar->context.index, isLeft) = currentCar;
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
					GET_CAR(workingSection, goingIn, oldPosInLine, isLeft) = NULL;
					GET_CAR(workingSection, goingIn, posInLine, flag) = currentCar;
					oldPosInLine = posInLine;
					
					currentCar->context.onLeftRoad = flag;
					currentCar->context.index = posInLine & 0xff;
					break;
				}
			}
		}
	}
}

//This function is super time critical and should be as optimized as possible
bool EDIIsExternalSlotAvailable(EDI_EXT_ROAD * workingSection, bool goingIn, uint posInLine, bool isLeft)
{
	return GET_CAR(workingSection, goingIn, posInLine, isLeft) == NULL;
}