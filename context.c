//
//  context.c
//  Crash'n Bob
//
//  Created by Taiki on 19/09/2015.
//  Copyright © 2015 Taiki. All rights reserved.
//

#include "main.h"

CONTEXT createContext()
{
	CONTEXT context = calloc(1, sizeof(CONTEXT_STRUCT));
	
	return context;
}

void finishedUpdateContext(CONTEXT context)
{
	if(!context->rendering.sorted)
	{
		qsort(context->cars, context->nbCars, sizeof(CAR *), sortCars);
		context->rendering.sorted = true;
	}
}

void destroyContext(CONTEXT context)
{
	while(context->nbCars-- > 0)
		crushCar(context->cars[context->nbCars]);
	
	free(context->cars);
	free(context);
}

//Go back to the begining
void contextNewPass(CONTEXT context)
{
	context->rendering.currentColumn = context->rendering.currentLine = context->rendering.currentIndex = 0;
	context->rendering.section = SECTION_NORTH;
	
	finishedUpdateContext(context);
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
	
	//Okay, we need to check if the rendering engine is asking for the next car of the list
	bool nextIsLegit = false;
	CAR nextCar = *context->cars[context->rendering.currentIndex];
	
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
			
		case SECTION_WEST:
		{
			byte section = nextCar.context.section;
			
			if(section != SECTION_WEST && section != SECTION_NODE && section != SECTION_EAST)
				break;
			
			//The next car is after the area where the W/E roads goes into the node
			if(section == SECTION_NODE && nextCar.context.lineOfNode > NB_SLOTS_BORDER + AREA_NODE_BEFORE_MERGE + WIDTH_NODE_MERGE)
				break;
			
			//We have a slot only used by the node in the middle
			uint lineOffset = context->rendering.currentLine - (NB_SLOTS_BORDER + AREA_NODE_BEFORE_MERGE);
			
			if(lineOffset != 2)
			{
				//We hide the empty slot in the middle
				//We do that because otherwise, the indexes of the cars on the external road and in the node would appear out of sync
				uint score = getScore(nextCar), column = context->rendering.currentColumn;
				if(score >= 2)
					++score;
				
				switch(section)
				{
					case SECTION_WEST:
					{
						if(4 - score == lineOffset && column < NB_SLOTS_BORDER)
						{
							if(nextCar.context.index == column)
								nextIsLegit = true;
						}
						
						break;
					}
						
					case SECTION_EAST:
					{
						if(score == lineOffset && column > NB_SLOTS_BORDER + 3)
						{
							column -= NB_SLOTS_BORDER + 3;
							
							if(NB_SLOTS_BORDER - nextCar.context.index == column)
								nextIsLegit = true;
						}
						break;
					}
						
					case SECTION_NODE:
					{
						if(nextCar.context.lineOfNode == context->rendering.currentLine - NB_SLOTS_BORDER		//At least, this is the good line
						   && nextCar.context.indexOfLineInNode == context->rendering.currentColumn - NB_SLOTS_BORDER)	//And good column \o/
						{
							nextIsLegit = true;
						}

						break;
					}
				}
				
				break;
			}
		}
			
		case SECTION_NODE:
		{
			if(nextCar.context.section != SECTION_NODE)
				break;
			
			uint currentLine = context->rendering.currentLine - NB_SLOTS_BORDER;
			
			if(nextCar.context.lineOfNode == currentLine		//At least, this is the good line
			   && nextCar.context.indexOfLineInNode == context->rendering.currentColumn)	//And good column \o/
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
		case NB_SLOTS_BORDER + AREA_NODE_BEFORE_MERGE + WIDTH_NODE_MERGE:
		{
			context->rendering.section = SECTION_NODE;
			break;
		}
			
		case NB_SLOTS_BORDER + AREA_NODE_BEFORE_MERGE:
		{
			context->rendering.section = SECTION_WEST;
			break;
		}
			
		case NB_SLOTS_BORDER + 2 * AREA_NODE_BEFORE_MERGE + WIDTH_NODE_MERGE:
		{
			context->rendering.section = SECTION_SOUTH;
			break;
		}
	}
}