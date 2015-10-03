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
	
	if(ctx == NULL)
	{
		printf("Memory error");
		exit(EXIT_FAILURE);
	}

	while(ctx->nbCars > 0)
	{
#ifdef DEBUG_BUILD
		for(uint i = 0; i < ctx->nbCars; ++i)
			printCar(ctx->cars[i]);
#endif
		drawGrid(ctx);

		EDIProcessContext(ctx);

		usleep(100000);
	}
	
	destroyContext(ctx);
	
	return 0;
}
