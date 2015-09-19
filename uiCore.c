//
//  uiCore.c
//  Crash'n Bob
//
//  Created by Taiki on 19/09/2015.
//  Copyright © 2015 Taiki. All rights reserved.
//

#include "main.h"

//This code is really ugly for a couple of reason:
//
//	- Why would anyone draw complex shapes in a shell ಠ_ಠ
//	- Lot of edge cases making functions not worth the trouble

void drawGrid()
{
	flushDisplay();
	
	CAR emptyDummy[2] = {getEmptyCar(), getEmptyCar()}, longEmpty[NB_SLOTS_BORDER + 5] = {getEmptyCar(), getEmptyCar(), getEmptyCar(), getEmptyCar(), getEmptyCar(), getEmptyCar(), getEmptyCar(), getEmptyCar(), getEmptyCar(), getEmptyCar(), getEmptyCar(), getEmptyCar(), getEmptyCar(), getEmptyCar(), getEmptyCar()};
	
	//Draw the top of the display
	uint offset = LENGTH_BORDER + LENGTH_NODE / 2 - WIDTH_ROAD_EXTERNAL / 2, internalSpace = WIDTH_ROAD_EXTERNAL;
	
	for(byte i = 0; i < NB_SLOTS_BORDER - 1; ++i)
	{
		printSpace(offset);
		printWideVerticalRoad(emptyDummy, emptyDummy);
		putc('\n', stdout);
	}
	
	offset -= 2 * CAR_WIDTH + 1;
	
	//Add the line at the top of
	printSpace(offset);
	fputs("_____", stdout);
	printWideVerticalRoad(emptyDummy, emptyDummy);
	puts("_____");
	
	//Top horizontal line
	printSpace(--offset);
	fputs("/  ", stdout);
	printHorizontalRoad(longEmpty, 7, true);
	puts("  \\");
	
	//Empty space between the two top lines
	printSpace(--offset);
	printf("/ %c", getCarReadableGlyph(getEmptyCar()));
	printChar(" - ", 7);
	printf("%c \\\n", getCarReadableGlyph(getEmptyCar()));
	
	//Internal line
	printSpace(--offset);
	fputs("/    ", stdout);
	printHorizontalRoad(longEmpty, 7, true);
	puts("    \\");
	
	//First oblique portion
	printSpace(--offset);
	printOblique45Road(getEmptyCar(), getEmptyCar());
	printChar("¯", internalSpace);
	printOblique135Road(getEmptyCar(), getEmptyCar());
	putc('\n', stdout);
	
	for(byte i = 0; i < 3; ++i)
	{
		printSpace(--offset);
		printOblique45Road(getEmptyCar(), getEmptyCar());
		internalSpace += 2;
		printSpace(internalSpace);
		printOblique135Road(getEmptyCar(), getEmptyCar());
		putc('\n', stdout);
	}
	
	printChar("_", --offset);
	printf("/ %c | %c ┃", getCarReadableGlyph(getEmptyCar()), getCarReadableGlyph(getEmptyCar()));
	printSpace(internalSpace);
	printf("┃ %c | %c \\", getCarReadableGlyph(getEmptyCar()), getCarReadableGlyph(getEmptyCar()));
	printChar("_", offset);
	putc('\n', stdout);

	//Top two lines
	for(byte i = 0; i < 2; ++i)
	{
		printHorizontalRoad(longEmpty, NB_SLOTS_BORDER + 1, true);
		printf(" ┃ %c ┃", getCarReadableGlyph(getEmptyCar()));
		printSpace(internalSpace);
		printf("┃ %c | ", getCarReadableGlyph(getEmptyCar()));
		printHorizontalRoad(longEmpty, NB_SLOTS_BORDER + 1, true);
		putc('\n', stdout);
	}
	
	//Separator
	printChar("-  ", offset / 3 + 1);
	printf("%c ┃ %c ┃", getCarReadableGlyph(getEmptyCar()), getCarReadableGlyph(getEmptyCar()));
	printSpace(internalSpace);
	printf("┃ %c | %c ", getCarReadableGlyph(getEmptyCar()), getCarReadableGlyph(getEmptyCar()));
	printChar(" - ", offset / 3 + 1);
	putc('\n', stdout);
	
	//Last two lines
	for(byte i = 0; i < 2; ++i)
	{
		printHorizontalRoad(longEmpty, NB_SLOTS_BORDER + 1, true);
		printf(" ┃ %c ┃", getCarReadableGlyph(getEmptyCar()));
		printSpace(internalSpace);
		printf("┃ %c | ", getCarReadableGlyph(getEmptyCar()));
		printHorizontalRoad(longEmpty, NB_SLOTS_BORDER + 1, true);
		putc('\n', stdout);
	}

	//Starting to close the area
	printChar("¯", offset);
	printf("\\ %c ┃ %c ┃", getCarReadableGlyph(getEmptyCar()), getCarReadableGlyph(getEmptyCar()));
	printSpace(internalSpace);
	printf("┃ %c | %c /", getCarReadableGlyph(getEmptyCar()), getCarReadableGlyph(getEmptyCar()));
	printChar("¯", offset);
	putc('\n', stdout);
	
	for(byte i = 0; i < 3; ++i)
	{
		printSpace(++offset);
		printOblique135Road(getEmptyCar(), getEmptyCar());
		printSpace(internalSpace);
		internalSpace -= 2;
		printOblique45Road(getEmptyCar(), getEmptyCar());
		putc('\n', stdout);
	}
	
	//First oblique portion
	printSpace(++offset);
	printOblique135Road(getEmptyCar(), getEmptyCar());
	printChar("_", internalSpace);
	printOblique45Road(getEmptyCar(), getEmptyCar());
	putc('\n', stdout);

	//Internal line
	printSpace(++offset);
	fputs("\\    ", stdout);
	printHorizontalRoad(longEmpty, 7, true);
	puts("    /");
	
	
	//Empty space between the two top lines
	printSpace(++offset);
	printf("\\ %c", getCarReadableGlyph(getEmptyCar()));
	printChar(" - ", 7);
	printf("%c /\n", getCarReadableGlyph(getEmptyCar()));
	
	//Top horizontal line
	printSpace(++offset);
	fputs("\\  ", stdout);
	printHorizontalRoad(longEmpty, 7, true);
	puts("  /");
	
	//Add the line at the top of
	printSpace(++offset);
	fputs("¯¯¯¯¯", stdout);
	printWideVerticalRoad(emptyDummy, emptyDummy);
	puts("¯¯¯¯¯");
	
	offset += 2 * CAR_WIDTH + 1;

	for(byte i = 0; i < NB_SLOTS_BORDER - 1; ++i)
	{
		printSpace(offset);
		printWideVerticalRoad(emptyDummy, emptyDummy);
		putc('\n', stdout);
	}
}