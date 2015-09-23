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
	context->cars[0].context.index = 3;
	context->cars[0].context.onLeftRoad = true;
	context->cars[0].context.section = SECTION_NORTH;// SECTION_NODE;
	context->cars[0] = updateNodeData(context->cars[0]);
	
	context->cars[1].direction = DIR_NORTH;
	context->cars[1].ID = 2;
	context->cars[1].isInitialized = true;
	context->cars[1].speed = SPEED_STANDARD;
	context->cars[1].status = STATUS_OK;
	context->cars[1].context.index = 7;
	context->cars[1].context.onLeftRoad = true;
	context->cars[1].context.section = SECTION_SOUTH;
	context->cars[1] = updateNodeData(context->cars[1]);
	
	context->nbCars = 2;
	
	return context;
}

void finishedUpdateContext(CONTEXT context)
{
	qsort(context->cars, context->nbCars, sizeof(CAR), sortCars);
	context->rendering.sorted = true;
}

void destroyContext(CONTEXT context)
{
	free(context->cars);
	free(context);
}

//Go back to the begining
void contextNewPass(CONTEXT context)
{
	context->rendering.currentColumn = context->rendering.currentLine = context->rendering.currentIndex = 0;
	context->rendering.section = SECTION_NORTH;
}

//Get car for the rendering engine
CAR contextGetNextCarForRendering(CONTEXT context)
{
	if(context->rendering.currentIndex >= context->nbCars)
	{
		//We don't really care about anything else at this point, since for us, the rendering is over
#ifdef DEV_VERSION
		fprintf(stderr, "Reached the end of the list");
#endif
		return getEmptyCar();
	}
	
	//Oops, someone forgot to call finishedUpdateContext
	if(!context->rendering.sorted)
		finishedUpdateContext(context);
	
	//Okay, we need to check if the rendering engine is asking for the next car of the list
	bool nextIsLegit = false;
	CAR nextCar = context->cars[context->rendering.currentIndex];
	
	switch (context->rendering.section)
	{
		case SECTION_NORTH:
		{
			if(nextCar.context.section != context->rendering.section)
				break;
			
			if(nextCar.context.index == context->rendering.currentLine		//At least, this is the good line
			   && getScore(nextCar) == context->rendering.currentColumn)	//And good column \o/
			{
				nextIsLegit = true;
			}

			break;
		}
			
		case SECTION_SOUTH:
		{
			if(nextCar.context.section != context->rendering.section)
				break;
			
			if(nextCar.context.index == HEIGHT - context->rendering.currentLine		//At least, this is the good line
			   && 3 - getScore(nextCar) == context->rendering.currentColumn)	//And good column \o/
			{
				nextIsLegit = true;
			}
			
			break;
		}
	}
	
	++context->rendering.currentColumn;
	
	if(!nextIsLegit)
		return getEmptyCar();

	//We jump to the next car
	++context->rendering.currentIndex;
	return nextCar;
}

//New line
void contextJumpNewLine(CONTEXT context)
{
	++context->rendering.currentLine;
	context->rendering.currentColumn = 0;
	
	switch(context->rendering.currentLine)
	{
		case NB_SLOTS_BORDER:
		case NB_SLOTS_BORDER + 8 + 5:
		{
			context->rendering.section = SECTION_NODE;
			break;
		}
			
		case NB_SLOTS_BORDER + 8:
		{
			context->rendering.section = SECTION_WEST;
			break;
		}
			
		case NB_SLOTS_BORDER + 2 * 8 + 5:
		{
			context->rendering.section = SECTION_SOUTH;
			break;
		}
	}
}