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
	
	//Then the node so we hopefully have space for cars wishing to get in
	for(uint pos = 0, nextForbidden = 0, forbiddenSlots[4] = EXTERNAL_SLOTS; pos < NB_SLOTS_NODE; ++pos)
	{
		if(nextForbidden >= 4 || forbiddenSlots[nextForbidden] != pos)
			EDIProcessCarInNode(context, pos, true);
		else
			++nextForbidden;
		
		EDIProcessCarInNode(context, pos, false);
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
#define GET_CAR(section, goingIn, posInLine, isLeft) (goingIn ? section->goingIn : section->goingOut)[posInLine][isLeft]

void EDIProcessCarInNode(CONTEXT context, uint posInLine, bool isLeft)
{
#warning "TODO"
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
	if(GET_CAR(workingSection, goingIn, posInLine, isLeft) == NULL)
		return;
	
	uint oldPosInLine = posInLine;
	CAR * currentCar = GET_CAR(workingSection, goingIn, posInLine, isLeft);
	
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