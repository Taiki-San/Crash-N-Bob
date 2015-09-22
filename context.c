//
//  context->c
//  Crash'n Bob
//
//  Created by Taiki on 19/09/2015.
//  Copyright © 2015 Taiki. All rights reserved.
//

#include "main.h"

CONTEXT createContext()
{
	CONTEXT context = calloc(1, sizeof(CONTEXT_STRUCT));
	
	context->cars = calloc(2, sizeof(CAR));
	
	context->cars[0].direction = DIR_SOUTH;
	context->cars[0].ID = 1;
	context->cars[0].isInitialized = true;
	context->cars[0].speed = SPEED_STANDARD;
	context->cars[0].status = STATUS_OK;
	context->cars[0].context.index = 12;
	context->cars[0].context.onLeftRoad = true;
	context->cars[0].context.section = SECTION_NODE;
	context->cars[0] = updateNodeData(context->cars[0]);
	
	context->cars[1].direction = DIR_SOUTH;
	context->cars[1].ID = 2;
	context->cars[1].isInitialized = true;
	context->cars[1].speed = SPEED_STANDARD;
	context->cars[1].status = STATUS_OK;
	context->cars[1].context.index = 7;
	context->cars[1].context.onLeftRoad = true;
	context->cars[1].context.section = SECTION_NORTH;
	context->cars[1] = updateNodeData(context->cars[1]);
	
	context->nbCars = 2;
	
	return context;
}

void finishedUpdateContext(CONTEXT context)
{
	qsort(context->cars, context->nbCars, sizeof(CAR), sortCars);
}

void destroyContext(CONTEXT context)
{
	free(context->cars);
	free(context);
}

//Go back to the begining
void contextNewPass(CONTEXT context)
{
	context->rendering.currentColumn = context->rendering.currentLine = 0;
}

//Get car for the rendering engine
CAR contextGetNextCarForRendering(CONTEXT context)
{
	++context->rendering.currentColumn;
	
	return context->cars[0];
}

//New line
void contextJumpNewLine(CONTEXT context)
{
	++context->rendering.currentLine;
	context->rendering.currentColumn = 0;
}