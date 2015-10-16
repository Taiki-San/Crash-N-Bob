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
#include <unistd.h>

#define DEBUG_BUILD

//Some custom types

typedef uint8_t byte;
typedef unsigned int uint;

//Define various constants

enum
{
	DIR_NORTH,
	DIR_WEST,
	DIR_EAST,
	DIR_SOUTH,
	DIR_RESERVED
};

enum
{
	SECTION_NORTH = DIR_NORTH,
	SECTION_WEST = DIR_WEST,
	SECTION_EAST = DIR_EAST,
	SECTION_SOUTH = DIR_SOUTH,
	SECTION_NODE = DIR_RESERVED,
	SECTION_MAX = SECTION_NODE,
	SECTION_EXTERNAL = 4
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
	HEIGHT = 40,
	WIDTH = 40,
	
	LENGTH_BORDER	= 33,
	LENGTH_NODE		= 29,
	WIDTH_ROAD_EXTERNAL = 14,
	
	CAR_WIDTH = 2,
	
	NB_SLOTS_BORDER = 10,
	
	NB_SLOTS_NODE = 54,
	
	AREA_NODE_BEFORE_MERGE = 8,
	WIDTH_NODE_MERGE = 5,
	
	DEFAULT_ACCIDENT_DELAY = 20
	
};

#define EXTERNAL_SLOTS	{6, 22, 33, 49}
#define ENTRY_SLOTS		{2, 16, 43, 29}
#define EXIT_SLOTS		{52, 12, 39, 25}

//Define general purpose structures

typedef struct
{
	uint ID;
	
	byte direction;
	
	byte speed;
	
	byte status;
	
	byte accidentDelay;
	
	bool isInitialized;
	
	struct {
		
		uint16_t index;
		
		byte section;
		
		bool onLeftRoad;
		
		byte lineOfNode;
		
		byte indexOfLineInNode;
		
	} context;
	
} CAR;

//Could be made slightly more memory efficient at the cost of access
//Considering the ridiculous amounts at stake, and how often I'll need to access, I go with minimizing time complexity
typedef struct thatIsForWhatHashTableAreGoodAtDammit
{
	CAR * goingIn[NB_SLOTS_BORDER][2];
	CAR * goingOut[NB_SLOTS_BORDER][2];
	
	byte section;
	
} EDI_EXT_ROAD;

typedef struct thisOneTooByTheWay
{
	CAR * node[NB_SLOTS_NODE][2];
	
} EDI_NODE;

typedef struct thisShouldBeAnObjectOnSoManyLevels
{
	CAR ** cars;
	uint nbCars;
	
	struct
	{
		uint currentLine;
		uint currentColumn;
		
		uint currentIndex;
		
		byte section;
		
		//We need the list to be sorted for the rendering pipeline to be efficient
		bool sorted;
		
	} rendering;
	
	struct
	{
		
		EDI_EXT_ROAD externalRoads[SECTION_EXTERNAL];
		EDI_NODE node;
		
	} EDI;

} CONTEXT_STRUCT;

typedef CONTEXT_STRUCT * CONTEXT;

//Functions declarations

//High level calls
void drawGrid(CONTEXT context);

//General utils
CAR getEmptyCar(void);
void printCar(const CAR * car);
void updateNodeData(CAR * car);
byte getScore(CAR car);
void ** removeItemAtIndexFromArray(void ** array, uint length, uint index);
uint getRandom(void);

//Car utils
CAR * createRandomCar(void);
byte carGetRandomDirection(void);
byte carGetRandomSectionDifferentOf(byte exception);
void crushCar(CAR * car);

//Context utils
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

//EDI utils
bool isValidSection(byte section);
bool EDIHaveFreeSlotToEnterSection(CONTEXT context, byte section);

//EDI
void EDIRegisterCarInContext(CONTEXT context, CAR * newCar);
void EDIRemoveCarFromContext(CONTEXT context, CAR * oldCar);
void EDIProcessContext(CONTEXT context);

bool EDIProcessCarInNode(CONTEXT context, uint posInLine, bool isLeft);
bool EDIIsCarInQuarterBeforeExit(uint16_t index, byte direction);
bool EDIIsNodeSlotAvailableFullCheck(EDI_NODE currentNode, uint posInLine, bool isLeft);
bool EDIIsNodeSlotAvailable(EDI_NODE currentNode, uint posInLine, bool isLeft);
bool EDIIsCarInFrontOfExit(uint16_t index, byte direction);
bool EDIIsCarOnLastStepExit(uint16_t index, byte direction);

void EDIProcessCarLeavingOnExternalRoad(CONTEXT context, EDI_EXT_ROAD * workingSection, uint posInLine, bool isLeft);
void EDIProcessCarEnteringOnExternalRoad(CONTEXT context, EDI_EXT_ROAD * workingSection, uint posInLine, bool isLeft);
void EDIProcessCarOnExternalRoad(CONTEXT context, EDI_EXT_ROAD * workingSection, bool goingIn, uint posInLine, bool isLeft);
bool EDIIsExternalSlotAvailable(EDI_EXT_ROAD * workingSection, bool goingIn, uint posInLine, bool isLeft);
bool EDIIsSlotReservedForExternalRing(uint index);
bool EDICarShouldMove(CAR * car);
bool EDITransitionCars(CAR ** car1, CAR ** car2);