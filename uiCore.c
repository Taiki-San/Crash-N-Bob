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
	fputs(ANSI_COLOR_BLUE "_____" ANSI_COLOR_RESET, stdout);
	printWideVerticalRoad(context, "\u2569");
	contextJumpNewLine(context);
	puts(ANSI_COLOR_BLUE "_____" ANSI_COLOR_RESET);
	
	//Top horizontal line
	printSpace(--offset);
	fputs(ANSI_COLOR_BLUE"/ "ANSI_COLOR_RESET, stdout);
	printHorizontalRoad(context, 11, false);
	contextJumpNewLine(context);
	puts(ANSI_COLOR_BLUE" \\"ANSI_COLOR_RESET);
	
	//Empty space between the two top lines
	printSpace(--offset);
	printf(ANSI_COLOR_BLUE"/"ANSI_COLOR_RESET" %c "ANSI_COLOR_CYAN, getCarReadableGlyph(context));
	printChar("\u2501\u2501\u2501", 6);
	printf(ANSI_COLOR_RESET"  %c "ANSI_COLOR_BLUE"\\"ANSI_COLOR_RESET"\n", getCarReadableGlyph(context));
	contextJumpNewLine(context);
	
	//Internal line
	printSpace(--offset);
	fputs(ANSI_COLOR_BLUE"/"ANSI_COLOR_RESET"   ", stdout);
	printHorizontalRoad(context, 11, false);
	contextJumpNewLine(context);
	puts("   "ANSI_COLOR_BLUE"\\"ANSI_COLOR_RESET);
	
	//First oblique portion
	printSpace(--offset);
	printOblique45Road(context);
	printChar(ANSI_COLOR_BLUE"¯"ANSI_COLOR_RESET, --internalSpace);
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
	
	printChar(ANSI_COLOR_BLUE"_"ANSI_COLOR_RESET, --offset);
	printf(ANSI_COLOR_BLUE"/"ANSI_COLOR_RESET"  %c "ANSI_COLOR_CYAN"\u2502"ANSI_COLOR_RESET" %c "ANSI_COLOR_BLUE"\u2502"ANSI_COLOR_RESET, getCarReadableGlyph(context), getCarReadableGlyph(context));
	printSpace(internalSpace);
	printf(ANSI_COLOR_BLUE"\u2502"ANSI_COLOR_RESET" %c "ANSI_COLOR_CYAN"\u2502"ANSI_COLOR_RESET" %c  "ANSI_COLOR_BLUE"\\"ANSI_COLOR_RESET, getCarReadableGlyph(context), getCarReadableGlyph(context));
	printChar(ANSI_COLOR_BLUE"_"ANSI_COLOR_RESET, offset);
	contextJumpNewLine(context);
	putc('\n', stdout);

	//Top two lines
	for(byte i = 0; i < 2; ++i)
	{
		printHorizontalRoad(context, NB_SLOTS_BORDER + 1, true);
		printf(ANSI_COLOR_CYAN" \u2502"ANSI_COLOR_RESET" %c "ANSI_COLOR_BLUE"\u2502"ANSI_COLOR_RESET, getCarReadableGlyph(context));
		printSpace(internalSpace);
		printf(ANSI_COLOR_BLUE"\u2502"ANSI_COLOR_RESET" %c "ANSI_COLOR_CYAN"\u2502 "ANSI_COLOR_RESET, getCarReadableGlyph(context));
		printHorizontalRoad(context, NB_SLOTS_BORDER + 1, true);
		contextJumpNewLine(context);
		putc('\n', stdout);
	}
	
	//Separator
	fputs(ANSI_COLOR_CYAN, stdout);
	printChar("\u2550\u2550\u2550", offset / 3);
	printf("\u2563  "ANSI_COLOR_RESET"%c "ANSI_COLOR_CYAN"\u2502"ANSI_COLOR_RESET" %c "ANSI_COLOR_BLUE"\u2502"ANSI_COLOR_RESET, getCarReadableGlyph(context), getCarReadableGlyph(context));
	printSpace(internalSpace);
	printf(ANSI_COLOR_BLUE"\u2502"ANSI_COLOR_RESET" %c "ANSI_COLOR_CYAN"\u2502"ANSI_COLOR_RESET" %c  "ANSI_COLOR_CYAN"\u2560\u2550", getCarReadableGlyph(context), getCarReadableGlyph(context));
	printChar("\u2550\u2550\u2550", offset / 3);
	contextJumpNewLine(context);
	puts(ANSI_COLOR_RESET);
	
	//Last two lines
	for(byte i = 0; i < 2; ++i)
	{
		printHorizontalRoad(context, NB_SLOTS_BORDER + 1, true);
		printf(ANSI_COLOR_CYAN" \u2502"ANSI_COLOR_RESET" %c "ANSI_COLOR_BLUE"\u2502"ANSI_COLOR_RESET, getCarReadableGlyph(context));
		printSpace(internalSpace);
		printf(ANSI_COLOR_BLUE"\u2502"ANSI_COLOR_RESET" %c "ANSI_COLOR_CYAN"\u2502 "ANSI_COLOR_RESET, getCarReadableGlyph(context));
		printHorizontalRoad(context, NB_SLOTS_BORDER + 1, true);
		contextJumpNewLine(context);
		putc('\n', stdout);
	}

	//Starting to close the area
	printChar(ANSI_COLOR_BLUE"¯"ANSI_COLOR_RESET, offset);
	printf(ANSI_COLOR_BLUE"\\"ANSI_COLOR_RESET"  %c "ANSI_COLOR_CYAN"\u2502"ANSI_COLOR_RESET" %c "ANSI_COLOR_BLUE"\u2502"ANSI_COLOR_RESET, getCarReadableGlyph(context), getCarReadableGlyph(context));
	printSpace(internalSpace);
	printf(ANSI_COLOR_BLUE"\u2502"ANSI_COLOR_RESET" %c "ANSI_COLOR_CYAN"\u2502"ANSI_COLOR_RESET" %c  "ANSI_COLOR_BLUE"/"ANSI_COLOR_RESET, getCarReadableGlyph(context), getCarReadableGlyph(context));
	printChar(ANSI_COLOR_BLUE"¯"ANSI_COLOR_RESET, offset);
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
	printChar(ANSI_COLOR_BLUE "_" ANSI_COLOR_RESET, internalSpace++);	//_
	printOblique45Road(context);
	contextJumpNewLine(context);
	putc('\n', stdout);

	//Internal line
	printSpace(++offset);
	fputs(ANSI_COLOR_BLUE"\\   "ANSI_COLOR_RESET, stdout);
	printHorizontalRoad(context, 11, false);
	contextJumpNewLine(context);
	puts(ANSI_COLOR_BLUE"   /"ANSI_COLOR_RESET);
	
	//Empty space between the two top lines
	printSpace(++offset);
	printf(ANSI_COLOR_BLUE"\\"ANSI_COLOR_RESET" %c "ANSI_COLOR_CYAN, getCarReadableGlyph(context));
	printChar("\u2501\u2501\u2501", 6);
	printf(ANSI_COLOR_RESET"  %c "ANSI_COLOR_BLUE"/"ANSI_COLOR_RESET"\n", getCarReadableGlyph(context));
	contextJumpNewLine(context);

	//Top horizontal line
	printSpace(++offset);
	fputs(ANSI_COLOR_BLUE"\\ "ANSI_COLOR_RESET, stdout);
	printHorizontalRoad(context, 11, false);
	contextJumpNewLine(context);
	puts(ANSI_COLOR_BLUE" /"ANSI_COLOR_RESET);
	
	//Add the line at the top of
	printSpace(++offset);
	fputs(ANSI_COLOR_BLUE"¯¯¯¯¯"ANSI_COLOR_RESET, stdout);
	printWideVerticalRoad(context, "\u2566");
	contextJumpNewLine(context);
	puts(ANSI_COLOR_BLUE"¯¯¯¯¯"ANSI_COLOR_RESET);
	
	offset += 2 * CAR_WIDTH + 1;

	for(byte i = 0; i < NB_SLOTS_BORDER - 1; ++i)
	{
		printSpace(offset);
		printWideVerticalRoad(context, "\u2551");
		contextJumpNewLine(context);
		putc('\n', stdout);
	}
}