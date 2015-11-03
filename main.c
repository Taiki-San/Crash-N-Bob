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
	CONTEXT ctx = createContext();
	bool dangerMode = getRandom() & 0x1;
	
	if(ctx == NULL)
	{
		printf("Memory error");
		exit(EXIT_FAILURE);
	}

	while(ctx->nbCars > 0)
	{
		if(ctx->nbCars < 9 && getRandom() % 4 == 0)
		{
			EDIRegisterCarInContext(ctx, createRandomCar(dangerMode));
			finishedUpdateContext(ctx);
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
		}
#endif

		EDIProcessContext(ctx);

		usleep(100000);
	} while(ctx->nbCars > 0);
	
	destroyContext(ctx);
	
	return 0;
}
