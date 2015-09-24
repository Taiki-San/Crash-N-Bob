//
//  EDICore.c
//  Crash'n Bob
//
//  Created by Taiki on 24/09/2015.
//  Copyright © 2015 Taiki. All rights reserved.
//

//Just for the record, EDI stand for Enhanced Driving Intelligence, nothing to see there

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
	
	context->cars = tmp;
	context->cars[context->nbCars++] = newCar;
	
	tmp = realloc(context->EDI.carsOnExternalRoadsIn, (context->EDI.nbCarsOnExternalRoadsIn + 1) * sizeof(CAR *));
	if(tmp == NULL)
	{
		printf("Memory error, couldn't update the context");
		return;
	}
	
	context->EDI.carsOnExternalRoadsIn = tmp;
	context->EDI.carsOnExternalRoadsIn[context->EDI.nbCarsOnExternalRoadsIn++] = newCar;
}

void EDIRemoveCarFromContext(CONTEXT context, CAR * oldCar)
{
	for (uint i = 0, length = context->nbCars; i < length; ++i)
	{
		if(context->cars[i] == oldCar)
		{
			context->cars = (CAR **) removeItemAtIndexFromArray((void **) context->cars, context->nbCars--, i);
			--context->nbCars;

			break;
		}
	}
}

void EDIProcessContext(CONTEXT context)
{
	uint i, length;
	CAR * currentCar;
	
	//If we had cars trying to get out, we process them first (easy)
	for(i = 0, length = context->EDI.nbCarsOnExternalRoadsOut; i < length; ++i)
	{
		currentCar = context->EDI.carsOnExternalRoadsOut[i];
		
		if(currentCar == NULL)
		{
#ifdef DEBUG_BUILD
			printf("Fragmented context :(");
#endif
		}
		
		//The car can't move for now
		else if(currentCar->status != STATUS_OK && currentCar->status != STATUS_DANGER);
		
		//The car is getting out \o/
		else if(currentCar->context.index == 0)
		{
			EDIRemoveCarFromContext(context, currentCar);
			context->EDI.carsOnExternalRoadsOut = (CAR **) removeItemAtIndexFromArray((void **) context->EDI.carsOnExternalRoadsOut, context->EDI.nbCarsOnExternalRoadsOut, i);

			length = --context->EDI.nbCarsOnExternalRoadsOut;
			--i;
		}
		
		//The car is just trying to move on
		else
		{
			
		}
	}
	
	//We should first process the cars within the circle
	
	finishedUpdateContext(context);
}