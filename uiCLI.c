//
//  uiCLI.c
//  Crash'n Bob
//
//  Created by Taiki on 03/11/2015.
//  Copyright © 2015 Taiki. All rights reserved.
//

#include "main.h"

byte getDifficulty()
{
	printf("************************************************************************\n");
	printf("**********      ****       ******       ******     ***** ***** *********\n");
	printf("********** ********* ****** **** ****** ****** ********* ***** *********\n");
	printf("********** ********* ****** **** ****** ****** ********* ***** *********\n");
	printf("********** ********* ***   *****        *******   ******       *********\n");
	printf("********** ********* *** ******* ****** ********** ***** ***** *********\n");
	printf("********** ********* **** ****** ****** ********** ***** ***** *********\n");
	printf("***********     **** ***** ***** ****** ******    ****** ***** *********\n");
	printf("************************************************************************\n");
	printf("*                 ---- Bienvenue dans Crash'n Bob ----                 *\n");
	printf("*      Le principe est simple, il s'agit de simuler un carrefour       *\n");
	printf("*      à sens giratoire comportant trois modes de fonctionnement       *\n");
	printf("************************************************************************\n");
	printf("************************************************************************\n");
	printf("************************************************************************\n");
	printf("*                            1. Mode fluide                            *\n");
	printf("*                            2. Mode chargé                            *\n");
	printf("*                  /!\\       3. Mode Danger         /!\\                *\n");
	printf("*                                                                      *\n");
	printf("*                            Q. Fermer le programme                    *\n");
	printf("************************************************************************\n");
	
	printf("Choissisez un mode:");

	char c;
	while (1)
	{
		while (scanf("%c", &c) != 1)
		{
			puts("Mode incorrect! Veuillez choisir un mode dans la liste!\n");
		}
		
		switch (c)
		{
			case '1':
				return DIFFICULTY_NORMAL;
				
			case '2':
				return DIFFICULTY_FAST;
				
			case '3':
				return DIFFICULTY_DANGER;
				
			case 'q':
			case 'Q':
				return DIFFICULTY_ERROR;
		}
	}
}
