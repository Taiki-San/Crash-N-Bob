//
//  main.h
//  Crash'n Bob
//
//  Created by Taiki on 16/09/2015.
//  Copyright © 2015 Taiki. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

#define DEBUG_BUILD

//Some custom types

typedef unsigned char byte;
typedef unsigned int uint;

//Define various constants

enum
{
	DIR_NORTH,
	DIR_WEST,
	DIR_RESERVED,
	DIR_EAST,
	DIR_SOUTH,
	DIR_MAX = DIR_WEST
};

enum
{
	SECTION_NORTH = DIR_NORTH,
	SECTION_WEST = DIR_WEST,
	SECTION_NODE = DIR_RESERVED,
	SECTION_EAST = DIR_EAST,
	SECTION_SOUTH = DIR_SOUTH
};

enum
{
	SPEED_STOPED,
	SPEED_STANDARD,
	SPEED_FAST,
	SPEED_MAX = SPEED_FAST
};

enum
{
	STATUS_OK,
	STATUS_DANGER,
	STATUS_DAMAGED,
	STATUS_FAILURE,
	STATUS_MAX = STATUS_FAILURE
};

//UI Configuration

enum
{
	LENGTH_BORDER	= 33,
	LENGTH_NODE		= 29,
	WIDTH_ROAD_EXTERNAL = 13,
	
	CAR_WIDTH = 2,
	
	NB_SLOTS_BORDER = 10,
	
	NB_SLOTS_NODE = 40,
};

//Those slots are only available in the external ring
#define SLOTS_EXTERNAL (byte [4]) {4, 13, 24, 33}

//Define general purpose structures

typedef struct
{
	uint ID;
	
	byte direction;
	
	byte speed;
	
	byte status;
	
	bool isInitialized;
	
	struct {
		
		byte index;
		
		byte section;
		
		bool onLeftRoad;
		
		byte lineOfNode;
		
		byte indexOfLineInNode;
		
	} context;
	
} CAR;

//Functions declarations

//High level calls
void drawGrid();

//General utils
CAR getEmptyCar();
void printCar(CAR car);
CAR updateNodeData(CAR car);

//UI
void printChar(char * string, uint nbChar);
void printSpace(uint nbSpace);
void flushDisplay();
char getCarReadableGlyph(CAR car);
void printWideVerticalRoad(CAR leftSide[2], CAR rightSide[2]);
void printHorizontalRoad(CAR data[], uint width, bool wider);
void printOblique45Road(CAR leftSide, CAR rightSide);
void printOblique135Road(CAR leftSide, CAR rightSide);
