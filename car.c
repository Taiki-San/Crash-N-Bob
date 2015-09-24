//
//  car.c
//  Crash'n Bob
//
//  Created by Taiki on 24/09/2015.
//  Copyright © 2015 Taiki. All rights reserved.
//

#include "main.h"

uint carCount = 0;

CAR * createRandomCar()
{
	CAR * output = malloc(sizeof(CAR));
	
	if(carCount == 0)
	{
		output->direction = DIR_SOUTH;
		output->status = STATUS_OK;
		output->context.index = 42;
		output->context.onLeftRoad = false;
		output->context.section = SECTION_NODE;
	}
	else if(carCount == 1)
	{
		output->direction = DIR_SOUTH;
		output->status = STATUS_OK;
		output->context.index = 42;
		output->context.onLeftRoad = true;
		output->context.section = SECTION_NODE;
	}
	else if(carCount == 2)
	{
		output->direction = DIR_EAST;
		output->status = STATUS_OK;
		output->context.index = 7;
		output->context.onLeftRoad = false;
		output->context.section = SECTION_NODE;
	}
	else
	{
		output->direction = carGetRandomDirection();
		output->status = (getRandom() & 0x7f) == 0 ? STATUS_DANGER : STATUS_OK;
		output->context.index = 0;
		output->context.onLeftRoad = getRandom() & 1;
		output->context.section = carGetRandomSectionDifferentOf(output->direction);
	}
	
	output->speed = (getRandom() & 0x3) == 0 ? SPEED_FAST : SPEED_STANDARD;
	output->ID = ++carCount;
	output->isInitialized = true;

	updateNodeData(output);
	
	return output;
}

byte carGetRandomDirection()
{
	switch (getRandom() & 0x3)
	{
		case 0:	return DIR_NORTH;
		case 1: return DIR_EAST;
		case 2: return DIR_SOUTH;
		default: return DIR_WEST;
	}
}

byte carGetRandomSectionDifferentOf(byte exception)
{
	byte output;
	
	while((output = carGetRandomDirection()) == exception);
	
	return output;
}

void crushCar(CAR * car)
{
	free(car);
}