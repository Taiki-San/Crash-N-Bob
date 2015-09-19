//
//  uiCore.c
//  Crash'n Bob
//
//  Created by Taiki on 19/09/2015.
//  Copyright © 2015 Taiki. All rights reserved.
//

#include "main.h"

void drawGrid()
{
	flushDisplay();
	
	CAR emptyDummy[2] = {getEmptyCar(), getEmptyCar()};
	
	//Draw the top of the display
	uint offset = LENGTH_BORDER + LENGTH_NODE / 2 - WIDTH_ROAD_EXTERNAL / 2;
	
	for(byte i = 0; i < NB_SLOTS_BORDER; ++i)
	{
		printSpace(offset);
		printVerticalRoad(emptyDummy, emptyDummy);
	}
}