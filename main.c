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
	void * ptr = malloc(42);
	if(ptr == NULL)
	{
		exit(EXIT_FAILURE);
	}
	
	for(i = 0; i < 300; ++i);
	return 0;
}
