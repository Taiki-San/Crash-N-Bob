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
#include <limits.h>

//Some custom types

typedef unsigned char byte;

//Define various constants

enum
{
	DIR_NORTH,
	DIR_EAST,
	DIR_SOUTH,
	DIR_WEST,
	DIR_MAX = DIR_WEST
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

//Define general purpose structures

typedef struct
{
	byte direction;
	
	byte speed;
	
	byte status;
	
} CAR;
