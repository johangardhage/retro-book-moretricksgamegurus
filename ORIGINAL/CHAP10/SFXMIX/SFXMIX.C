/******************************************************************************
*File:			sfxmix.c
*Copyright: 1995 DiamondWare, Ltd.	All rights reserved.
*Written: 	Erik Lorenzen & Keith Weiner
*Purpose: 	Contains code to play & mix digitized sounds
******************************************************************************/



#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "sound.h"
#include "env.h"
#include "dig.h"
#include "load.h"



void main(int argc, char **argv)
{
	env_BLASTER  blaster;
	SOUND 			 sound[16];
	INT8				 x;
	INT8				 y;

	printf("\nSFXMIX Written by Erik Lorenzen & Keith Weiner\n");
	printf("Copyright 1995 DiamondWare, Ltd.\n");
	printf("All rights reserved.\n\n\n");

	if (argc < 2)
	{
		printf("Usage SFXMIX <wav-file>  (up to 15 of 'em)\n");
		exit(-1);
	}

	if (argc >= 16) 								//15 wave files
	{
		printf("Gimme a break!\n");
		exit(-1);
	}

	if (!env_GetBlaster(&blaster))
	{
		printf("Unable to get BLASTER settings!\n");
		exit(-1);
	}
	else
	{
		printf("BLASTER settings are: A%x I%u D%u\n\n", blaster.ioaddr,
					 blaster.irqlev, blaster.dmachan);
	}

	for (x=1;x<argc;x++)
	{
		sound[x].samples = NULL;			//waste the first one...
	}

	for (x=1;x<argc;x++)
	{
		if (!load_Wave(argv[x], &sound[x]))
		{
			goto DUMPOUT;
		}

		printf("Press %c to play this sound\n", x + 'A' - 1);
	}

	printf("Press Q to exit\n");

	if (!dig_Init(&blaster))
	{
		goto DUMPOUT;
	}

	do
	{
		y = (INT8)getch();

		for (x=1;x<argc;x++)
		{
			if (toupper(y) == (x + 'A' - 1))
			{
				dig_Play(&sound[x]);
			}
		}

	} while (toupper(y) != 'Q');

	dig_Kill();

	DUMPOUT:

	for (x=1;x<argc;x++)
	{
		if (sound[x].samples != NULL)
		{
			free(sound[x].samples);
		}
	}
}
