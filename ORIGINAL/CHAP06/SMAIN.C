#include <stdio.h>
#include <stdlib.h>
#include <mem.h>
#include <conio.h>

#include "palette.h"
#include "pcx.h"
#include "dostime.h"


/*****************************************************************
 * Type definitions
 ****************************************************************/
typedef unsigned char UBYTE;
typedef unsigned short UWORD;
typedef signed short WORD;
typedef unsigned long ULONG;


/*****************************************************************
 * Global variables
 ****************************************************************/
PALETTE palette;
int width, height;


/*****************************************************************
 * External Functions
 *****************************************************************/
void cdecl ScaleBlit( UBYTE *bitmap, int x0, int y0,
	int x1, int y1);


/*****************************************************************
 * Functions
 *****************************************************************/
int getvmode( void );
#pragma aux getvmode =\
    "mov    ah,0x0F",\
    "int    0x10,"\
    "and	eax,0xFF",\

int setvmode(int);
#pragma aux setvmode =\
	"int 0x10",\
	parm [eax]\


void main()
{
	char *fname = "ROCKET.PCX";
	UBYTE *rocketBitmap = NULL;
	int nOldMode;
    int nResult;
	int width2, height2;
	ULONG startTime, stopTime;
	int fps, frames = 0;

	nResult = ReadPCX256(fname, &palette, &width, &height,
		&rocketBitmap);
	if ( nResult != pcxOkay )
	{
		printf("Error reading %s\n", fname);
		exit(1);
	}

	width2 = width;
	height2 = height;

	nOldMode = getvmode();
	setvmode(0x13);
	gSetDACRange(0, 256, palette);

	startTime = GetDosTicks();

	while ( 1 )
	{
		if ( kbhit() )
			break;

        width2 = 2;
		height2 = 2;

		while (height2 < 200 && width2 < 320)
		{
			ScaleBlit(
				rocketBitmap,		// bitmap
				160 - width2 / 2,	// x0
				100 - height2 / 2,	// y0
				160 + width2 / 2,	// x1
				100 + height2 / 2	// y1
			);

			height2 += 2;
			width2 += 2;

			frames++;
		}
	}

	stopTime = GetDosTicks();

	setvmode(nOldMode);

	getch();

	fps = frames * 182 / (stopTime-startTime);
	printf("Average frame rate: %i.%i fps\n", fps / 10, fps % 10);
}


















