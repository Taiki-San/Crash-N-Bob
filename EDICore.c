//
//  EDICore.c
//  Crash'n Bob
//
//  Created by Taiki on 24/09/2015.
//  Copyright © 2015 Taiki. All rights reserved.
//

//Just for the record, EDI stand for Enhanced Driving System, nothing to see there

#include "main.h"

void EDIRegisterCarInContext(CONTEXT context, CAR * newCar)
{
	if(context->nbCars == UINT_MAX)
	{
		printf("Too many cars!");
		crushCar(newCar);
		return;
	}
	
	void * tmp = realloc(context->cars, (context->nbCars + 1) * sizeof(CAR *));
	if(tmp == NULL)
	{
		printf("Memory error, couldn't add car to array");
		crushCar(newCar);
		return;
	}
	
	context->cars[context->nbCars++] = newCar;
}

void EDIProcessContext(CONTEXT context)
{
	finishedUpdateContext(context);
}