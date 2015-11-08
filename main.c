//
//  main.c
//  Crash'n Bob
//
//  Created by Taiki on 16/09/2015.
//  Copyright © 2015 Taiki. All rights reserved.
//

#include "main.h"

int main(int argc, const char * argv[])
{
#ifndef DEBUG_BUILD
	byte difficulty = getDifficulty();
	if(difficulty == DIFFICULTY_ERROR)
		return -1;
#else
	byte difficulty = 1;
#endif
	
	CONTEXT ctx = createContext();
	
	ctx->isFastMode = difficulty & 0x1;
	ctx->isDangerMode = (difficulty & 0x2) != 0;
	
	if(ctx == NULL)
	{
		printf("Memory error");
		exit(EXIT_FAILURE);
	}

	do
	{
		if(shouldInjectCar(ctx))
		{
			EDIRegisterCarInContext(ctx, createRandomCar(ctx->isDangerMode));
		}
		
#ifdef DEBUG_BUILD
		puts("\n\nDumping all the data \\o/\n");
		for(uint i = 0; i < ctx->nbCars; ++i)
			printCar(ctx->cars[i]);
#endif
		drawGrid(ctx);
		
#ifdef DEBUG_BUILD
		if(ctx->rendering.currentIndex != ctx->nbCars)
		{
			puts("Not so fast! We missed a couple of cars");
			finishedUpdateContext(ctx);
			drawGrid(ctx);
		}
#endif

		EDIProcessContext(ctx);

		usleep(100000);
	} while(ctx->nbCars > 0);
	
	destroyContext(ctx);
	
	return 0;
}
