//
//  uiCore.c
//  Crash'n Bob
//
//  Created by Taiki on 19/09/2015.
//  Copyright © 2015 Taiki. All rights reserved.
//

#include "main.h"

//
//This code is really ugly for a couple of reason:
//
//	- Why would anyone draw complex shapes in a shell ಠ_ಠ
//	- Lot of edge cases making functions not worth the trouble
//	- There is very little logic there, so it's not worth the extra work
//
//

void drawGrid(CONTEXT context)
{
	flushDisplay();
	contextNewPass(context);
	
	//Draw the top of the display
	uint offset = LENGTH_BORDER + LENGTH_NODE / 2 - WIDTH_ROAD_EXTERNAL / 2, internalSpace = WIDTH_ROAD_EXTERNAL;
	
	for(byte i = 0; i < NB_SLOTS_BORDER - 1; ++i)
	{
		printSpace(offset);
		printWideVerticalRoad(context);
		contextJumpNewLine(context);
		putc('\n', stdout);
	}
	
	offset -= 2 * CAR_WIDTH + 1;
	
	//Add the line at the top of
	printSpace(offset);
	fputs("_____", stdout);
	printWideVerticalRoad(context);
	contextJumpNewLine(context);
	puts("_____");
	
	//Top horizontal line
	printSpace(--offset);
	fputs("/ ", stdout);
	printHorizontalRoad(context, 11, false);
	contextJumpNewLine(context);
	puts(" \\");
	
	//Empty space between the two top lines
	printSpace(--offset);
	printf("/ %c", getCarReadableGlyph(context));
	printChar(" - ", 7);
	printf("%c \\\n", getCarReadableGlyph(context));
	contextJumpNewLine(context);
	
	//Internal line
	printSpace(--offset);
	fputs("/   ", stdout);
	printHorizontalRoad(context, 11, false);
	contextJumpNewLine(context);
	puts("   \\");
	
	//First oblique portion
	printSpace(--offset);
	printOblique45Road(context);
	printChar("¯", internalSpace);
	printOblique135Road(context);
	contextJumpNewLine(context);
	putc('\n', stdout);
	
	for(byte i = 0; i < 3; ++i)
	{
		printSpace(--offset);
		printOblique45Road(context);
		internalSpace += 2;
		printSpace(internalSpace);
		printOblique135Road(context);
		contextJumpNewLine(context);
		putc('\n', stdout);
	}
	
	printChar("_", --offset);
	printf("/ %c ┃ %c ┃", getCarReadableGlyph(context), getCarReadableGlyph(context));
	printSpace(internalSpace);
	printf("┃ %c | %c \\", getCarReadableGlyph(context), getCarReadableGlyph(context));
	printChar("_", offset);
	contextJumpNewLine(context);
	putc('\n', stdout);

	//Top two lines
	for(byte i = 0; i < 2; ++i)
	{
		printHorizontalRoad(context, NB_SLOTS_BORDER + 1, true);
		printf(" ┃ %c ┃", getCarReadableGlyph(context));
		printSpace(internalSpace);
		printf("┃ %c | ", getCarReadableGlyph(context));
		printHorizontalRoad(context, NB_SLOTS_BORDER + 1, true);
		contextJumpNewLine(context);
		putc('\n', stdout);
	}
	
	//Separator
	printChar("-  ", offset / 3 + 1);
	printf("%c ┃ %c ┃", getCarReadableGlyph(context), getCarReadableGlyph(context));
	printSpace(internalSpace);
	printf("┃ %c | %c ", getCarReadableGlyph(context), getCarReadableGlyph(context));
	printChar(" - ", offset / 3 + 1);
	contextJumpNewLine(context);
	putc('\n', stdout);
	
	//Last two lines
	for(byte i = 0; i < 2; ++i)
	{
		printHorizontalRoad(context, NB_SLOTS_BORDER + 1, true);
		printf(" ┃ %c ┃", getCarReadableGlyph(context));
		printSpace(internalSpace);
		printf("┃ %c | ", getCarReadableGlyph(context));
		printHorizontalRoad(context, NB_SLOTS_BORDER + 1, true);
		contextJumpNewLine(context);
		putc('\n', stdout);
	}

	//Starting to close the area
	printChar("¯", offset);
	printf("\\ %c ┃ %c ┃", getCarReadableGlyph(context), getCarReadableGlyph(context));
	printSpace(internalSpace);
	printf("┃ %c | %c /", getCarReadableGlyph(context), getCarReadableGlyph(context));
	printChar("¯", offset);
	contextJumpNewLine(context);
	putc('\n', stdout);
	
	for(byte i = 0; i < 3; ++i)
	{
		printSpace(++offset);
		printOblique135Road(context);
		printSpace(internalSpace);
		internalSpace -= 2;
		printOblique45Road(context);
		contextJumpNewLine(context);
		putc('\n', stdout);
	}
	
	//First oblique portion
	printSpace(++offset);
	printOblique135Road(context);
	printChar("_", internalSpace);
	printOblique45Road(context);
	contextJumpNewLine(context);
	putc('\n', stdout);

	//Internal line
	printSpace(++offset);
	fputs("\\   ", stdout);
	printHorizontalRoad(context, 11, false);
	contextJumpNewLine(context);
	puts("   /");
	
	//Empty space between the two top lines
	printSpace(++offset);
	printf("\\ %c", getCarReadableGlyph(context));
	printChar(" - ", 7);
	printf("%c /\n", getCarReadableGlyph(context));
	contextJumpNewLine(context);

	//Top horizontal line
	printSpace(++offset);
	fputs("\\ ", stdout);
	printHorizontalRoad(context, 11, false);
	contextJumpNewLine(context);
	puts(" /");
	
	//Add the line at the top of
	printSpace(++offset);
	fputs("¯¯¯¯¯", stdout);
	printWideVerticalRoad(context);
	contextJumpNewLine(context);
	puts("¯¯¯¯¯");
	
	offset += 2 * CAR_WIDTH + 1;

	for(byte i = 0; i < NB_SLOTS_BORDER - 1; ++i)
	{
		printSpace(offset);
		printWideVerticalRoad(context);
		contextJumpNewLine(context);
		putc('\n', stdout);
	}
}