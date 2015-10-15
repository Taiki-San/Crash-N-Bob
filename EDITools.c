//
//  EDITools.c
//  Crash'n Bob
//
//  Created by Taiki on 02/10/2015.
//  Copyright © 2015 Taiki. All rights reserved.
//

#include "main.h"

bool isValidSection(byte section)
{
	return section < SECTION_MAX;
}

bool EDIHaveFreeSlotToEnterSection(CONTEXT context, byte section)
{
	if(context == NULL)
		return false;
	
	if(section >= SECTION_EXTERNAL)
		section %= SECTION_EXTERNAL;
	
	return context->EDI.externalRoads[section].goingIn[0][0] == NULL || context->EDI.externalRoads[section].goingIn[0][1] == NULL;
}

bool EDIIsSlotReservedForExternalRing(uint index)
{
	for(byte i = 0; i < 4; ++i)
	{
		if(((byte [4]) EXTERNAL_SLOTS)[i] == index)
			return true;
	}
	
	return false;
}

bool EDICarShouldMove(CAR * car)
{
	if(car == NULL || !car->isInitialized)
		return false;
	
	if(car->status == STATUS_OK || car->status == STATUS_DANGER)
		return true;
	
	if(car->status == STATUS_DAMAGED)
	{
		car->accidentDelay--;
		if(car->accidentDelay == 0)
		{
			car->status = STATUS_OK;
			return true;
		}
	}
	
	return false;
}

void EDITransitionCars(CAR ** car1, CAR ** car2)
{
	//No move
	if(car1 == car2 || *car1 == *car2)
		return;
	
	if(*car2 == NULL)
	{
		*car2 = *car1;
		*car1 = NULL;
	}
	else
	{
		(*car1)->status = STATUS_DAMAGED;
		(*car1)->accidentDelay = DEFAULT_ACCIDENT_DELAY;
		
		(*car2)->status = STATUS_DAMAGED;
		(*car2)->accidentDelay = DEFAULT_ACCIDENT_DELAY;
	}
}