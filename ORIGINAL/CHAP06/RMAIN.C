#include <stdio.h>
#include <stdlib.h>
#include <mem.h>
#include <conio.h>
#include <math.h>

#include "palette.h"
#include "pcx.h"
#include "dostime.h"

/*****************************************************************
 * Defines
 ****************************************************************/
#define kAngle360		256
#define kAngleMask		(kAngle360 - 1)
#define kAngle180		(kAngle360 / 2)
#define kAngle90		(kAngle360 / 4)
#define kPi				3.141592654

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
long sinTable[kAngle360];

/*****************************************************************
 * External Functions
 *****************************************************************/
void cdecl RotateBlit(UBYTE *bitmap, int angle, long scale);


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


void InitSinTable()
{
	int i;

	for (i = 0; i < kAngle180; i++)
		sinTable[i] = sin((double)i * kPi / kAngle180) * 0x10000L;

	for (i = kAngle180; i < kAngle360; i++)
		sinTable[i] = -sinTable[i - kAngle180];
}


void main()
{
	char *fname = "ROCKET.PCX";
	UBYTE *rocketBitmap = NULL;
	int nOldMode;
    int nResult;
	int angle;
	ULONG startTime, stopTime;
	int fps, frames = 0;

	InitSinTable();

	nResult = ReadPCX256(fname, &palette, &width, &height,
		&rocketBitmap);
	if ( nResult != pcxOkay )
	{
		printf("Error reading %s\n", fname);
		exit(1);
	}

	nOldMode = getvmode();
	setvmode(0x13);
	gSetDACRange(0, 256, palette);

	startTime = GetDosTicks();

	while ( 1 )
	{
		if ( kbhit() )
			break;

		for (angle = 0; angle < kAngle360; angle++)
		{
			RotateBlit(rocketBitmap, angle,
				sinTable[angle] / 2 + 0x9000);
			frames++;
		}
	}

	stopTime = GetDosTicks();

	setvmode(nOldMode);

	getch();

	fps = frames * 182 / (stopTime-startTime);
	printf("Average frame rate: %i.%i fps\n", fps / 10, fps % 10);
}


















