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
	fprintf(stderr, "Debug data:\n	ID: %u\n	direction: %d\n	speed: %d\n	status: %d\n	isInitialized: %d\n	index: %d\n	section: %d\n	onLeftRoad: %d", car.ID, car.direction, car.speed, car.status, car.isInitialized, car.context.index, car.context.section, car.context.onLeftRoad);
}

CAR updateNodeData(CAR car)
{
	if(car.context.section != SECTION_NODE)
		return car;
	
	byte index = car.context.index;
	byte externalOffsets[4] = SLOTS_EXTERNAL;
	
	//Magic table to convert an index into external circle coordonate
	byte conversionTable[40][2] = {{0, 7}, {0, 6}, {0, 5}, {0, 4}, {1, 3}, {2, 2}, {3, 1}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 1}, {12, 2}, {13, 3}, {14, 4}, {14, 5}, {14, 6}, {14, 7}, {14, 8}, {14, 9}, {14, 10}, {13, 11}, {12, 12}, {11, 13}, {10, 14}, {9, 14}, {8, 14}, {7, 14}, {6, 14}, {5, 14}, {4, 14}, {3, 13}, {2, 12}, {1, 11}, {0, 10}, {0, 9}, {0, 8}};
	
	car.context.lineOfNode = conversionTable[index][0];
	car.context.indexOfLineInNode = conversionTable[index][1];
	
	//Internal circle :(
	if(car.context.onLeftRoad)
	{
		if(index < externalOffsets[0] || index > externalOffsets[3])
			++car.context.lineOfNode;
		
		else if(index < externalOffsets[1])
			++car.context.indexOfLineInNode;
		
		else if(index < externalOffsets[2])
			--car.context.lineOfNode;
		
		else
			--car.context.indexOfLineInNode;
	}
	
	return car;
}