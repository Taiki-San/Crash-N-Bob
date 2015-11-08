//
//  uiTools.c
//  Crash'n Bob
//
//  Created by Taiki on 16/09/2015.
//  Copyright © 2015 Taiki. All rights reserved.
//

#include "main.h"

//Toolbox

char getCarReadableGlyph(CONTEXT context)
{
	CAR car = contextGetNextCarForRendering(context);
	
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

void printChar(char * string, uint nbChar)
{
	while(nbChar-- > 0)
		fputs(string, stdout);
}

void printWideVerticalRoad(CONTEXT context, char * separator)
{
	printf(COLOR_BORDER"\u2502"COLOR_CAR" %c %c "COLOR_SEPARATOR"%s"COLOR_CAR" %c %c "COLOR_BORDER"\u2502", getCarReadableGlyph(context), getCarReadableGlyph(context), separator, getCarReadableGlyph(context), getCarReadableGlyph(context));
}

void printHorizontalRoad(CONTEXT context, uint width, bool wider)
{
	if(width == 0)
		return;
	
	fputs(COLOR_CAR, stdout);
	putc(getCarReadableGlyph(context), stdout);
	
	for(uint i = 1; i < width; ++i)
		printf(wider ? "  %c" : " %c", getCarReadableGlyph(context));
}

void printOblique45Road(CONTEXT context)
{
	printf(COLOR_BORDER"/"COLOR_CAR" %c "COLOR_SEPARATOR"/"COLOR_CAR" %c "COLOR_BORDER"/", getCarReadableGlyph(context), getCarReadableGlyph(context));
}

void printOblique135Road(CONTEXT context)
{
	printf(COLOR_BORDER"\\"COLOR_CAR" %c "COLOR_SEPARATOR"\\"COLOR_CAR" %c "COLOR_BORDER"\\", getCarReadableGlyph(context), getCarReadableGlyph(context));
}

//Sorting work
int sortCars(const void * _a, const void * _b)
{
	const CAR * a = * (void **) _a, * b = * (void **) _b;
	
	if(!a->isInitialized || !b->isInitialized)
		return a->isInitialized ? -1 : 1;

	if(a->context.section == b->context.section)
	{
		bool priorityLeft = false;
		
		switch (a->context.section)
		{
			case SECTION_NORTH:
			{
				//If not at the same point, the closest of the border of the display win
				if(a->context.index != b->context.index)
					return (int) a->context.index - (int) b->context.index;
				
				priorityLeft = true;
			}
				
			case SECTION_SOUTH:
			{
				if(a->context.index != b->context.index)
					return (a->context.index - b->context.index) * (priorityLeft ? 1 : -1);

				//Checking the index of the given row
				uint scoreA = getScore(*a), scoreB = getScore(*b);
				if(scoreA != scoreB)
					return (scoreA > scoreB ? -1 : 1) * (priorityLeft ? -1 : 1);
				
				//WTF?! There should be a colision
#ifdef DEBUG_BUILD
				fputs("Collision detected, WTF?!", stderr);
				printCar(a);
				printCar(b);
#endif
				return 0;
			}
				
			case SECTION_EAST:
			{
				priorityLeft = true;
			}
				
			case SECTION_WEST:
			{
				//We determine a score depending of the line
				uint scoreA = getScore(*a), scoreB = getScore(*b);
				
				//Not the same line
				if(scoreA != scoreB)
					return (scoreA > scoreB ? -1 : 1) * (priorityLeft ? -1 : 1);
				
				if(a->context.index != b->context.index)
					return (a->context.index - b->context.index) * (priorityLeft ? -1 : 1);
				
				//WTF?! There should be a colision
#ifdef DEBUG_BUILD
				fputs("Collision detected, WTF?!", stderr);
				printCar(a);
				printCar(b);
#endif
				return 0;
			}
				
			case SECTION_NODE:
			{
				if(a->context.lineOfNode != b->context.lineOfNode)
					return a->context.lineOfNode - b->context.lineOfNode;
				
				return a->context.indexOfLineInNode - b->context.indexOfLineInNode;
			}
		}
	}
	
	//Not in the same section but possibly on the same line
	if(a->context.section == SECTION_NODE || b->context.section == SECTION_NODE)
	{
		if((b->context.section == SECTION_WEST || b->context.section == SECTION_EAST) ^ (a->context.section == SECTION_WEST || a->context.section == SECTION_EAST))
		{
			const CAR * nodeCar, * otherCar;
			int ifNodeWin;
			if(a->context.section == SECTION_NODE)
			{
				nodeCar = a;
				otherCar = b;
				ifNodeWin = -1;
			}
			else
			{
				nodeCar = b;
				otherCar = a;
				ifNodeWin = 1;
			}
			
			//Over or under the entry
			if(nodeCar->context.lineOfNode < 8)
				return ifNodeWin;
			
			else if(nodeCar->context.lineOfNode > 12)
				return ifNodeWin * -1;
			
			//Geting tricky
			uint lineCode = getScore(*otherCar);
			if(otherCar->context.section == SECTION_WEST)
				lineCode = 3 - lineCode;
			
			//There is an empty line in the middle of the external road
			if(lineCode > 1)
				++lineCode;
			
			lineCode += 8;	//Offset
			
			//Node car is below the other one
			if(lineCode < nodeCar->context.lineOfNode)
				return ifNodeWin * -1;
			
			//Node car is over the other one
			else if(lineCode > nodeCar->context.lineOfNode)
				return ifNodeWin;
			
			//Node car is on the same line, but after the other one
			else if(otherCar->context.section == SECTION_WEST)
				return ifNodeWin * -1;
			
			//Same line, 
			return ifNodeWin;
		}
		else if(a->context.section == SECTION_NODE)
			return b->context.section == SECTION_SOUTH ? -1 : 1;
		else
			return a->context.section == SECTION_SOUTH ? 1 : -1;
	}
	
	else if((a->context.section == SECTION_EAST || a->context.section == SECTION_WEST) && (b->context.section == SECTION_EAST || b->context.section == SECTION_WEST))
	{
		//Okay, in this case, we have one car in SECTION_EAST, and one in SECTION_WEST
		
		//We determine a score depending of the line
		uint scoreA = getScore(*a), scoreB = getScore(*b);
		
		if(a->context.section == SECTION_WEST)
			scoreA = 3 - scoreA;
		else
			scoreB = 3 - scoreB;
		
		if(scoreA != scoreB)
			return scoreA > scoreB ? 1 : -1;
		
		//And on the same line, the standard computation is enough to choose
	}
	
	return a->context.section - b->context.section;
}

inline byte getScore(CAR car)
{
	if(car.direction == car.context.section)
		return 2 + (car.context.onLeftRoad ? 0 : 1);
	
	return car.context.onLeftRoad;
}