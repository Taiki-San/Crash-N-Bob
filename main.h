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
	HEIGHT = 41,
	WIDTH = 41,
	
	LENGTH_BORDER	= 33,
	LENGTH_NODE		= 29,
	WIDTH_ROAD_EXTERNAL = 13,
	
	CAR_WIDTH = 2,
	
	NB_SLOTS_BORDER = 10,
	
	NB_SLOTS_NODE = 54,
	
	AREA_NODE_BEFORE_MERGE = 8,
	WIDTH_NODE_MERGE = 5
	
};

#define EXTERNAL_SLOTS {6, 22, 33, 49}

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

typedef struct thisShouldBeAnObjectOnSoManyLevels
{
	CAR * cars;
	uint nbCars;
	
	struct
	{
		byte currentLine;
		byte currentColumn;
		
		byte currentIndex;
		
		byte section;
		
		//We need the list to be sorted for the rendering pipeline to be efficient
		bool sorted;
		
	} rendering;

} CONTEXT_STRUCT;

typedef CONTEXT_STRUCT * CONTEXT;

//Functions declarations

//High level calls
void drawGrid(CONTEXT context);

//General utils
CAR getEmptyCar(void);
void printCar(CAR car);
CAR updateNodeData(CAR car);
byte getScore(CAR car);
uint getRandom(void);

//Car utils
CAR createRandomCar(void);
byte carGetRandomDirection(void);
byte carGetRandomSectionDifferentOf(byte exception);

//Context Utils
CONTEXT createContext(void);
void destroyContext(CONTEXT context);
void contextNewPass(CONTEXT context);
CAR contextGetNextCarForRendering(CONTEXT context);
void contextJumpNewLine(CONTEXT context);
void finishedUpdateContext(CONTEXT context);
int sortCars(const void * _a, const void * _b);

//UI
void printChar(char * string, uint nbChar);
void printSpace(uint nbSpace);
void flushDisplay(void);
char getCarReadableGlyph(CONTEXT context);
void printWideVerticalRoad(CONTEXT context);
void printHorizontalRoad(CONTEXT context, uint width, bool wider);
void printOblique45Road(CONTEXT context);
void printOblique135Road(CONTEXT context);

//EDI
void EDIProcessContext(CONTEXT context);
