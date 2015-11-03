//
//  car.c
//  Crash'n Bob
//
//  Created by Taiki on 24/09/2015.
//  Copyright © 2015 Taiki. All rights reserved.
//

#include "main.h"

uint carCount = 0;

CAR * createRandomCar(bool inDangerMode)
{
	CAR * output = malloc(sizeof(CAR));
	
	output->direction = carGetRandomDirection();
	output->status = (!inDangerMode || getRandom() & 0x7f) == 0 ? STATUS_DANGER : STATUS_OK;
	output->speed = (getRandom() & 0x3) == 0 ? SPEED_FAST : SPEED_STANDARD;
	output->ID = ++carCount;
	output->isInitialized = true;
	
	memset(&output->context, 0, sizeof(output->context));
	
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