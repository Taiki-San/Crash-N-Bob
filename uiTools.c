//
//  uiTools.c
//  Crash'n Bob
//
//  Created by Taiki on 16/09/2015.
//  Copyright © 2015 Taiki. All rights reserved.
//

#include "main.h"

//Toolbox

void printSpace(uint nbSpace)
{
	while(nbSpace-- > 0)
		putc(' ', stdout);
}

void printVerticalRoad()
{
	fputs("| ", stdout);
}