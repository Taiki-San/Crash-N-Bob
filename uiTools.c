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

void printSpace(uint nbSpace)
{
	while(nbSpace-- > 0)
		putc(' ', stdout);
}

void printVerticalRoad(CAR leftSide[2], CAR rightSide[2])
{
	printf("┃%c %c | %c %c┃\n", getCarReadableGlyph(leftSide[0]), getCarReadableGlyph(leftSide[1]), getCarReadableGlyph(rightSide[0]), getCarReadableGlyph(rightSide[1]));
}

void printOblique45Road(CAR leftSide, CAR rightSide)
{
	printf("/%c / %c /\n", getCarReadableGlyph(leftSide), getCarReadableGlyph(rightSide));
}

void printOblique1355Road(CAR leftSide, CAR rightSide)
{
	printf("\\%c \\ %c \\\n", getCarReadableGlyph(leftSide), getCarReadableGlyph(rightSide));
}