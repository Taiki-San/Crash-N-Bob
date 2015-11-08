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
		printWideVerticalRoad(context, "\u2551");
		contextJumpNewLine(context);
		putc('\n', stdout);
	}
	
	offset -= 2 * CAR_WIDTH + 1;
	
	//Add the line at the top of
	printSpace(offset);
	fputs(COLOR_BORDER "_____" , stdout);
	printWideVerticalRoad(context, "\u2569");
	contextJumpNewLine(context);
	puts(COLOR_BORDER "_____" );
	
	//Top horizontal line
	printSpace(--offset);
	fputs(COLOR_BORDER"/ ", stdout);
	printHorizontalRoad(context, 11, false);
	contextJumpNewLine(context);
	puts(COLOR_BORDER" \\");
	
	//Empty space between the two top lines
	printSpace(--offset);
	printf(COLOR_BORDER"/"COLOR_CAR" %c "COLOR_SEPARATOR, getCarReadableGlyph(context));
	printChar("\u2501\u2501\u2501", 6);
	printf(COLOR_CAR"  %c "COLOR_BORDER"\\""\n", getCarReadableGlyph(context));
	contextJumpNewLine(context);
	
	//Internal line
	printSpace(--offset);
	printf(COLOR_BORDER"/ "COLOR_CAR"%c "COLOR_SEPARATOR"/ ", getCarReadableGlyph(context));
	printHorizontalRoad(context, 9, false);
	printf(COLOR_SEPARATOR" \\"COLOR_CAR" %c "COLOR_BORDER"\\\n", getCarReadableGlyph(context));
	contextJumpNewLine(context);
	
	//First oblique portion
	printSpace(--offset);
	printOblique45Road(context);
	printChar(COLOR_BORDER"¯", --internalSpace);
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
	
	printChar(COLOR_BORDER"_", --offset);
	printf(COLOR_BORDER"/"COLOR_CAR"  %c "COLOR_SEPARATOR"\u2502"COLOR_CAR" %c "COLOR_BORDER"\u2502", getCarReadableGlyph(context), getCarReadableGlyph(context));
	printSpace(internalSpace);
	printf(COLOR_BORDER"\u2502"COLOR_CAR" %c "COLOR_SEPARATOR"\u2502"COLOR_CAR" %c  "COLOR_BORDER"\\", getCarReadableGlyph(context), getCarReadableGlyph(context));
	printChar(COLOR_BORDER"_", offset);
	contextJumpNewLine(context);
	fputs("\n"COLOR_SEPARATOR, stdout);

	//Top two lines
	for(byte i = 0; i < 2; ++i)
	{
		printHorizontalRoad(context, NB_SLOTS_BORDER + 1, true);
		printf(COLOR_SEPARATOR" \u2502"COLOR_CAR" %c "COLOR_BORDER"\u2502", getCarReadableGlyph(context));
		printSpace(internalSpace);
		printf("\u2502"COLOR_CAR" %c "COLOR_SEPARATOR"\u2502 ", getCarReadableGlyph(context));
		printHorizontalRoad(context, NB_SLOTS_BORDER + 1, true);
		contextJumpNewLine(context);
		putc('\n', stdout);
	}
	
	//Separator
	fputs(COLOR_SEPARATOR, stdout);
	printChar("\u2550\u2550\u2550", offset / 3);
	printf("\u2563  "COLOR_CAR"%c "COLOR_SEPARATOR"\u2502"COLOR_CAR" %c "COLOR_BORDER"\u2502", getCarReadableGlyph(context), getCarReadableGlyph(context));
	printSpace(internalSpace);
	printf(COLOR_BORDER"\u2502"COLOR_CAR" %c "COLOR_SEPARATOR"\u2502"COLOR_CAR" %c  "COLOR_SEPARATOR"\u2560\u2550", getCarReadableGlyph(context), getCarReadableGlyph(context));
	printChar("\u2550\u2550\u2550", offset / 3);
	contextJumpNewLine(context);
	putc('\n', stdout);
	
	//Last two lines
	for(byte i = 0; i < 2; ++i)
	{
		printHorizontalRoad(context, NB_SLOTS_BORDER + 1, true);
		printf(COLOR_SEPARATOR" \u2502"COLOR_CAR" %c "COLOR_BORDER"\u2502", getCarReadableGlyph(context));
		printSpace(internalSpace);
		printf("\u2502"COLOR_CAR" %c "COLOR_SEPARATOR"\u2502 ", getCarReadableGlyph(context));
		printHorizontalRoad(context, NB_SLOTS_BORDER + 1, true);
		contextJumpNewLine(context);
		putc('\n', stdout);
	}

	//Starting to close the area
	printChar(COLOR_BORDER"¯", offset);
	printf(COLOR_BORDER"\\"COLOR_CAR"  %c "COLOR_SEPARATOR"\u2502"COLOR_CAR" %c "COLOR_BORDER"\u2502", getCarReadableGlyph(context), getCarReadableGlyph(context));
	printSpace(internalSpace);
	printf(COLOR_BORDER"\u2502"COLOR_CAR" %c "COLOR_SEPARATOR"\u2502"COLOR_CAR" %c  "COLOR_BORDER"/", getCarReadableGlyph(context), getCarReadableGlyph(context));
	printChar(COLOR_BORDER"¯", offset);
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
	
	//Last oblique portion
	printSpace(++offset);
	printOblique135Road(context);
	printChar(COLOR_BORDER "_" , internalSpace++);
	printOblique45Road(context);
	contextJumpNewLine(context);
	putc('\n', stdout);

	//Internal line
	printSpace(++offset);
	printf(COLOR_BORDER"\\ "COLOR_CAR"%c "COLOR_SEPARATOR"\\ ", getCarReadableGlyph(context));
	printHorizontalRoad(context, 9, false);
	printf(COLOR_SEPARATOR" /"COLOR_CAR" %c "COLOR_BORDER"/\n", getCarReadableGlyph(context));
	contextJumpNewLine(context);
	
	//Empty space between the two top lines
	printSpace(++offset);
	printf(COLOR_BORDER"\\"COLOR_CAR" %c "COLOR_SEPARATOR, getCarReadableGlyph(context));
	printChar("\u2501\u2501\u2501", 6);
	printf(COLOR_CAR"  %c "COLOR_BORDER"/""\n", getCarReadableGlyph(context));
	contextJumpNewLine(context);

	//Top horizontal line
	printSpace(++offset);
	fputs(COLOR_BORDER"\\ ", stdout);
	printHorizontalRoad(context, 11, false);
	contextJumpNewLine(context);
	puts(COLOR_BORDER" /");
	
	//Add the line at the top of
	printSpace(++offset);
	fputs(COLOR_BORDER"¯¯¯¯¯", stdout);
	printWideVerticalRoad(context, "\u2566");
	contextJumpNewLine(context);
	puts(COLOR_BORDER"¯¯¯¯¯");
	
	offset += 2 * CAR_WIDTH + 1;

	for(byte i = 0; i < NB_SLOTS_BORDER - 1; ++i)
	{
		printSpace(offset);
		printWideVerticalRoad(context, "\u2551");
		contextJumpNewLine(context);
		putc('\n', stdout);
	}
}