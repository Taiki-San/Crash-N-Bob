//
//  uiTools.c
//  Crash'n Bob
//
//  Created by Taiki on 16/09/2015.
//  Copyright © 2015 Taiki. All rights reserved.
//

#include "main.h"

//Toolbox

char getCarReadableGlyph(CAR car)
{
	//Empty space
	if(!car.isInitialized)
		return ' ';
	
	//Border cases
	switch (car.status)
	{
		case STATUS_DAMAGED:
			return 'A';
			
		case STATUS_DANGER:
			return 'D';
			
		case STATUS_FAILURE:
			return 'P';
	}
	
	//The car is fine, we just need its goal
	switch (car.direction)
	{
		case DIR_NORTH:
			return 'n';
			
		case DIR_EAST:
			return 'e';
			
		case DIR_WEST:
			return 'w';
			
		case DIR_SOUTH:
			return 's';
	}
	
	//Shouldn't happen, but can make the issue visible
	return '@';
}

//Basic printing routines

void flushDisplay()
{
	system("clear");
}

void printSpace(uint nbSpace)
{
	while(nbSpace-- > 0)
		putc(' ', stdout);
}

void printVerticalRoad(CAR leftSide[2], CAR rightSide[2])
{
	printf("┃ %c %c | %c %c ┃\n", getCarReadableGlyph(leftSide[0]), getCarReadableGlyph(leftSide[1]), getCarReadableGlyph(rightSide[0]), getCarReadableGlyph(rightSide[1]));
}

void printOblique45Road(CAR leftSide, CAR rightSide)
{
	printf("/%c / %c /\n", getCarReadableGlyph(leftSide), getCarReadableGlyph(rightSide));
}

void printOblique135Road(CAR leftSide, CAR rightSide)
{
	printf("\\%c \\ %c \\\n", getCarReadableGlyph(leftSide), getCarReadableGlyph(rightSide));
}

//Sorting work
int sortCars(void * _a, void * _b)
{
	CAR * a = _a, * b = _b;
	
	if(!a->isInitialized || !b->isInitialized)
		return a->isInitialized ? -1 : 1;

	if(a->context.section != b->context.section)
		return b->context.section - a->context.section;
	
	bool cachedOnLeft, priorityLeft = false;

	switch (a->context.section)
	{
		case SECTION_NORTH:
		{
			//If not at the same point, the closest of the border of the display win
			if(a->context.index != b->context.index)
				return b->context.index - a->context.index;

			priorityLeft = true;
		}
		
		case SECTION_SOUTH:
		{
			if(a->context.index != b->context.index)
				return a->context.index - b->context.index;
			
			//Depending of the destination, we could be going to the border of the display
			cachedOnLeft = a->direction == a->context.section;
			if(cachedOnLeft != (b->direction == a->context.section))
				return !(cachedOnLeft ^ priorityLeft) ? -1 : 1;
			
			//Same stage
			cachedOnLeft = a->context.onLeftRoad;
			if(cachedOnLeft != b->context.onLeftRoad)
				return !(cachedOnLeft ^ priorityLeft) ? -1 : 1;
			
			//WTF?! There should be a colision
#ifdef DEBUG_BUILD
			fputs("Collision detected, WTF?!", stderr);
			printCar(*a);
			printCar(*b);
#endif
			break;
		}
			
		case SECTION_EAST:
		{
			priorityLeft = true;
		}
			
		case SECTION_WEST:
		{
			//We determine a score depending of the line
			uint scoreA = ((a->direction == a->context.section) ? 2 : 0) + (a->context.onLeftRoad ? 0 : 1);
			uint scoreB = ((b->direction == a->context.section) ? 2 : 0) + (b->context.onLeftRoad ? 0 : 1);
			
			//Not the same line
			if(scoreA != scoreB)
				return (scoreA > scoreB ? -1 : 1) * (priorityLeft ? -1 : 1);

			if(a->context.index != b->context.index)
				return (b->context.index - a->context.index) * (priorityLeft ? -1 : 1);

			//WTF?! There should be a colision
#ifdef DEBUG_BUILD
			fputs("Collision detected, WTF?!", stderr);
			printCar(*a);
			printCar(*b);
#endif
			break;
		}
			
		//And now, the fun begin...
		case SECTION_NODE:
		{
			
		}
	}
	
	return 0;
}