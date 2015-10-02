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
	byte baseSection = carGetRandomSectionDifferentOf(newCar->context.section);
	for(byte i = 0; i < SECTION_EXTERNAL - 1; ++i)
	{
		if(EDIHaveFreeSlotToEnterSection(context, baseSection + i))
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
		
		for(uint pos = 0; pos < NB_SLOTS_BORDER; ++pos)
		{
			EDIProcessCarEnteringOnExternalRoad(context, workingSection, pos, true);
			EDIProcessCarEnteringOnExternalRoad(context, workingSection, pos, false);
		}
	}
	
	finishedUpdateContext(context);
}

#pragma mark - Node processing

void EDIProcessCarInNode(CONTEXT context, uint posInLine, bool isLeft)
{
#warning "TODO"
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
	if((goingIn ? workingSection->goingIn : workingSection->goingOut)[posInLine][isLeft] == NULL)
		return;
	
	CAR * currentCar = (goingIn ? workingSection->goingIn : workingSection->goingOut)[posInLine][isLeft];
	
	//The car can't move for now
	
	if(currentCar->status != STATUS_OK && currentCar->status != STATUS_DANGER);
	
	//The car is getting out \o/
	else if(!goingIn && posInLine == 0)
		EDIRemoveCarFromContext(context, currentCar);
	
#warning "TODO"
	else if(goingIn && posInLine == NB_SLOTS_NODE)
	{
		
	}

	else
	{
		char delta = goingIn ? 1 : -1;
	}
}

//This function is super time critical and should be as optimized as possible
bool EDIIsExternalSlotAvailable(EDI_EXT_ROAD * workingSection, bool goingIn, uint posInLine, bool isLeft)
{
	return (goingIn ? workingSection->goingIn : workingSection->goingOut)[posInLine][isLeft] == NULL;
}