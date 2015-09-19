//
//  utils.c
//  Crash'n Bob
//
//  Created by Taiki on 19/09/2015.
//  Copyright © 2015 Taiki. All rights reserved.
//

#include "main.h"

CAR getEmptyCar()
{
	CAR car;
	
	memset(&car, 0, sizeof(CAR));
	
	//Would be needed is memset had to be remove
//	car.isInitialized = false;
	
	return car;
}