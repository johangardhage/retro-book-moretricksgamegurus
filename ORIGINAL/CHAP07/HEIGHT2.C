#include <stdarg.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mem.h>
#include <dos.h>
#include <math.h>
#include <fcntl.h>
#include <io.h>

#include "palette.h"
#include "dostime.h"
#include "key.h"
#include "pcx.h"

/*****************************************************************
 * Defines
 ****************************************************************/
#define kPi				3.141592654

#define	kFieldBits		9
#define kFieldSize		(1 << kFieldBits)
#define kFieldBytes		(kFieldSize * kFieldSize)

#define kRendBits		8
#define kRendSize		(1 << kRendBits)

#define kMaxSpeed		(100 << 8)
#define kAccel			16
#define kMaxAltitude	1000

#define kAngle360		0x1000
#define kAngleMask		(kAngle360 - 1)
#define kAngle180		(kAngle360 / 2)
#define kAngle90		(kAngle360 / 4)
#define kAngle60		(kAngle360 / 6)
#define kAngle45		(kAngle360 / 8)
#define kAngle30		(kAngle360 / 12)
#define kViewAngle   	(kAngle60/2)	// half view cone angle

#define SIN(x)			sinTable[(x) & kAngleMask]
#define COS(x)			sinTable[((x) + kAngle90) & kAngleMask]

#define kDefaultPitch	(kRendSize / 2)
#define kRollAccel		32
#define kSkyHeight		60				// scale sky height

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
long 		sinTable[kAngle360];
UBYTE 		gHeightMap[kFieldBytes];
UBYTE 		gColorMap[kFieldBytes];
int 		xOrg = 128L << 16;
int			yOrg = 128L << 16;
char 		gRendBuffer[kRendSize * kRendSize];
long		gAltitude = 220;			// arbitrary start height
int 		gYaw = kAngle90;
int			gRollAngle = 0;
int			gPitch = kDefaultPitch;		// horizon line
char		*cloudMap = NULL;
int			gSkyHeight = kSkyHeight;
PALETTE		palette;


/***********************************************************************
 * External declarations
 **********************************************************************/
void cdecl CastRay(int col, int horiz, int dx, int dy);
void cdecl CopyRotate(void);
void cdecl RipHoriz8(int y, int u0, int v0, int u1, int v1, char *texture);


/*****************************************************************
 * Inline functions to get and set video mode
 ****************************************************************/
int getvmode( void );
#pragma aux getvmode =\
    "mov    ah,0x0F",\
    "int    0x10,"\
    "and	eax,0xFF",\

int setvmode(int);
#pragma aux setvmode =\
	"int 0x10",\
	parm [eax]\


/*****************************************************************
 * Quit the program with a variable arg message.
 ****************************************************************/
void QuitMessage(char * fmt, ...)
{
	char msg[80];
	va_list argptr;
	va_start( argptr, fmt );
	vsprintf( msg, fmt, argptr );
	va_end(argptr);
	setvmode(0x3);
	printf(msg);
	exit(1);
}


/*****************************************************************
 * Initialize the sin table.  Values are 16:16 fixed point.
 ****************************************************************/
void InitMathTables()
{
	int i;
	for (i = 0; i < kAngle180; i++)
		sinTable[i] = sin((double)i * kPi / kAngle180) * 0x10000L;

	for (i = kAngle180; i < kAngle360; i++)
		sinTable[i] = -sinTable[i - kAngle180];
}


/*****************************************************************
 * Load in the height and color fields.
 ****************************************************************/
void InitHeightField()
{
	UBYTE *heightMap = gHeightMap;
	UBYTE *colorMap = gColorMap;
	int width, height;
	int r;

	width = height = kFieldSize;
	r = ReadPCX256("demoh.pcx", NULL, &width, &height,
		&heightMap);
	if (r != pcxOkay)
		QuitMessage("Error reading height map");

	width = height = kFieldSize;
	r = ReadPCX256("democ.pcx", palette, &width, &height,
		&colorMap);
	if (r != pcxOkay)
		QuitMessage("Unable to read height map");
}


int CalcOffset(int x, int y);
#pragma aux CalcOffset = \
	"mov	ebx,edx" \
	"shr	ebx,32-9" \
	"shld	ebx,ecx,9" \
	parm [ECX][EDX] \
	value [EBX];


#define RADIX (32 - kFieldBits)

/*****************************************************************
 * Render the view.  RADIX defines the radix point for coordinates
 * in the height and color fields.  Once the view is rendered, it
 * is translated affinely to the screen with CopyRotate().
 ****************************************************************/
void UpdateScreen()
{
	long dx, dy;
	int col, angle;
	int i;
	long dist,lsin,lcos,rsin,rcos;
	long u0,v0,u1,v1;
	int skyHoriz = gPitch + 20;	// overlap clipping of terrain

	// draw the texture mapped sky
	lsin = sinTable[(gYaw + kViewAngle) & kAngleMask];
	lcos = sinTable[(gYaw + kViewAngle + kAngle90) & kAngleMask];
	rsin = sinTable[(gYaw - kViewAngle) & kAngleMask];
	rcos = sinTable[(gYaw - kViewAngle + kAngle90) & kAngleMask];

	for ( i = 0; i < skyHoriz && i < kRendSize; i++)
	{

		dist = (gSkyHeight << 8) / (skyHoriz - i);
		u0 = (lcos >> 8) * dist + (xOrg >> 10);
		v0 = (lsin >> 8) * dist + (yOrg >> 10);
		u1 = (rcos >> 8) * dist + (xOrg >> 10);
		v1 = (rsin >> 8) * dist + (yOrg >> 10);

		RipHoriz8(i, u0, v0, u1, v1, cloudMap);
	}

	for ( col = 0; col < kRendSize; col++ )
	{
		angle = (kViewAngle * (kRendSize - col * 2)) >> kRendBits;
		dx = COS(gYaw + angle) << (RADIX - 16);
		dy = SIN(gYaw + angle) << (RADIX - 16);

		// the -40 on the pitch forces the view down to see more of the terrain
		CastRay(col, gPitch - 40, dx, dy);
	}

	CopyRotate();
}


/*****************************************************************
 * Process keystrokes.  This is a pretty poor flight model, but
 * does manage to show off the abilities of the rendering code.
 ****************************************************************/
int CheckKeys()
{
	static speed = 0x200;

	if ( keyGet(KEY_ESC) )
		return -1;

	if (keyTest(KEY_PAGEUP))
		gSkyHeight++;

	if (keyTest(KEY_PAGEDN))
		gSkyHeight--;

	if (keyTest(KEY_PLUS))
		speed += kAccel;

	if (keyTest(KEY_MINUS))
		speed -= kAccel;

	if (speed < -kMaxSpeed) speed = -kMaxSpeed;
	if (speed > kMaxSpeed) speed = kMaxSpeed;

	if ( keyTest(KEY_UP) || keyTest(KEY_DOWN) )
	{
		if (keyTest(KEY_UP))
			gPitch -= 4;

		if (keyTest(KEY_DOWN))
			gPitch += 4;
	}
	else
	{
		if ( gPitch < kDefaultPitch )
			gPitch++;
		else if ( gPitch > kDefaultPitch )
			gPitch--;
	}

	if (keyTest(KEY_LEFT) || keyTest(KEY_RIGHT))
	{
		if (keyTest(KEY_LEFT))
			gRollAngle = (gRollAngle - kRollAccel) & kAngleMask;
		if (keyTest(KEY_RIGHT))
			gRollAngle = (gRollAngle + kRollAccel) & kAngleMask;
	}
	else
	{
		if ( gRollAngle > 0 && gRollAngle < kAngle180 )
			gRollAngle = (gRollAngle - kRollAccel / 2) & kAngleMask;
		else if ( gRollAngle > kAngle180 && gRollAngle < kAngle360 )
			gRollAngle = (gRollAngle + kRollAccel / 2) & kAngleMask;
	}

	gYaw  = (gYaw - (SIN(gRollAngle) >> 11)) & kAngleMask;
	gAltitude += (speed * (gPitch - kDefaultPitch)) >> 14;
	gAltitude = min(gAltitude, kMaxAltitude);
	xOrg += speed * COS(gYaw) >> 8;
	yOrg += speed * SIN(gYaw) >> 8;

	return 0;
}


/*****************************************************************
 * Main program.  This shell loads the terrain, calls the main
 * rendering loop, and does some quick and dirty benchmarking.
 ****************************************************************/
void main(void)
{
	int width = 0, height = 0;
	UBYTE *background = NULL;
	ULONG startTime, stopTime;
	int fps10, frames = 0;
	int nOldMode;
	int r;

	printf("Initializing math tables...\n");
	InitMathTables();

	printf("Loading terrain...\n");
	InitHeightField();

	printf("Loading cloud map...\n");
	r = ReadPCX256("clouds.pcx", NULL, &width, &height, &cloudMap);
	if (r != pcxOkay)
		QuitMessage("Unable to load cloud map.");

	width = 320;
	height = 200;

	r = ReadPCX256("backgrnd.pcx", NULL, &width, &height,
		&background);
	if (r != pcxOkay)
		QuitMessage("Unable to load background.");

	keyInstall();		// install low-level keyboard handler

	nOldMode = getvmode();
	setvmode(0x13);

	gSetDACRange(0, 256, palette);

	memcpy((UBYTE *)0xA0000, background, 320 * 200);

	startTime = GetDosTicks();
	while ( CheckKeys() != -1 ) {
		UpdateScreen();
		frames++;
	}
	stopTime = GetDosTicks();

	setvmode(nOldMode);

	keyRemove();		// remove low-level keyboard handler

	fps10 = frames * 182 / (stopTime - startTime);
	printf("Height Mapping Demo ");
	printf("Copyright (c) 1995 Peter M. Freese\n");
	printf("Average frame rate: %i.%i fps\n", fps10/10, fps10%10);
}

