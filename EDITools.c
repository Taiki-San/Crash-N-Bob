//
//  EDITools.c
//  Crash'n Bob
//
//  Created by Taiki on 02/10/2015.
//  Copyright © 2015 Taiki. All rights reserved.
//

#include "main.h"

bool isValidSection(byte section)
{
	return section < SECTION_MAX;
}

bool EDIHaveFreeSlotToEnterSection(CONTEXT context, byte section)
{
	if(context == NULL)
		return false;
	
	if(section >= SECTION_EXTERNAL)
		section %= SECTION_EXTERNAL;
	
	return context->EDI.externalRoads[section].goingIn[0][0] == NULL || context->EDI.externalRoads[section].goingIn[0][1] == NULL;
}