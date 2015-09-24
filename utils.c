//
//  utils.c
//  Crash'n Bob
//
//  Created by Taiki on 19/09/2015.
//  Copyright © 2015 Taiki. All rights reserved.
//

#include "main.h"

CAR getEmptyCar()
{
	CAR car;
	
	car.isInitialized = false;
	
	return car;
}

void printCar(CAR car)
{
	fprintf(stderr, "Debug data:\n	ID: %u\n	direction: %d\n	speed: %d\n	status: %d\n	isInitialized: %d\n	index: %d\n	section: %d\n	onLeftRoad: %d\n", car.ID, car.direction, car.speed, car.status, car.isInitialized, car.context.index, car.context.section, car.context.onLeftRoad);
}

CAR updateNodeData(CAR car)
{
	if(car.context.section != SECTION_NODE)
		return car;
	
	byte index = car.context.index;
	byte externalOffsets[4] = EXTERNAL_SLOTS;	//Slots only available in the external ring
	
	//Magic table to convert an index into external circle coordonate
	byte conversionTable[54][2] = {{0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0}, {12, 0}, {13, 0}, {14, 0}, {16, 0}, {17, 0}, {19, 0}, {20, 0}, {20, 1}, {20, 2}, {20, 3}, {20, 4}, {20, 5}, {20, 6}, {20, 7}, {20, 8}, {20, 9}, {20, 10}, {19, 1}, {17, 3}, {16, 3}, {15, 3}, {14, 3}, {13, 3}, {12, 3}, {11, 3}, {10, 3}, {9, 3}, {8, 3}, {7, 3}, {6, 3}, {5, 3}, {4, 3}, {3, 3}, {1, 1}, {0, 9}, {0, 8}, {0, 7}, {0, 6}};
	
	car.context.lineOfNode = conversionTable[index][0];
	car.context.indexOfLineInNode = conversionTable[index][1];
	
	//Internal circle :(
	if(car.context.onLeftRoad)
	{
		if(index < externalOffsets[0] || index > externalOffsets[3])
			car.context.lineOfNode += 2;
		
		else if(index < externalOffsets[1])
			++car.context.indexOfLineInNode;
		
		else if(index < externalOffsets[2])
			car.context.lineOfNode -= 2;
		
		else
			--car.context.indexOfLineInNode;
	}
	
	return car;
}

#ifndef __APPLE__

#include <time.h>

static bool seeded = false;

#endif

uint getRandom()
{
#ifdef __APPLE__
	return arc4random();
#else
	if(!seeded)
	{
		FILE * file = fopen("/dev/urandom", "rb");
		uint seed;
		
		if(file != NULL)
		{
			fread(&seed, 1, sizeof(uint), file);
			fclose(file);
		}
		else
			seed = (uint) time(NULL);
		
		srand(seed);
		seeded = true;
	}
	
	return (uint) rand();
#endif
}