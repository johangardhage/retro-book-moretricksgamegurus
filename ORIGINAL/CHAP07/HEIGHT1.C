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

#define kRendHeight		200
#define kRendWidth		320

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

#define kDefaultPitch	(kRendHeight / 2)
#define kYawAccel		(kAngle360 / 256)


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
char 		gRendBuffer[kRendWidth * kRendHeight];
long		gAltitude = 220;			// arbitrary start height
int 		gYaw = kAngle90;
int			gPitch = kDefaultPitch;		// horizon line
PALETTE		palette;


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


/*****************************************************************
 * Calculate a texel address from u,v coordinates.
 ****************************************************************/
int CalcOffset(int u, int v);
#pragma aux CalcOffset = \
	"mov	ebx,edx" \
	"shr	ebx,32-9" \
	"shld	ebx,ecx,9" \
	parm [ECX][EDX] \
	value [EBX];


#define kVScale			0x600
#define kDepthClip		(256 * kVScale)


/*****************************************************************
 * Main rendering loop.  Shoots a ray from the origin along the
 * specified vector.
 ****************************************************************/
void CastRay(int col, int horiz, int dx, int dy)
{
	int x, y, z, dz, h, ph;
	int pixel, offset;
	UBYTE c;

	dz = (horiz - (kRendHeight - 1)) * kVScale;

	// point to bottom of the column
	pixel = col + (kRendHeight - 1) * kRendWidth;

	// initial projected pixel height
	ph = 0;

	// initial ray height
	z = gAltitude << 16;

	// initial coordinates
	x = xOrg << (16 - kFieldBits);
	y = yOrg << (16 - kFieldBits);

	while (ph < kDepthClip)
	{
		y += dy;
		x += dx;
		z += dz;
		ph += kVScale;

		// calculate the offset in the height field
		offset = CalcOffset(x, y);

		h = gHeightMap[offset] << 16;

		// an intersection occured
		if (h > z)
		{
			c = gColorMap[offset];
			do
			{
				// increment slope
				dz += kVScale;
				gRendBuffer[pixel] = c;	// draw the pixel
				z += ph;
				pixel -= kRendWidth;	// move up one line
				if (pixel < 0)
					return;
			} while ( h > z);
		}
	}
}


#define RADIX (32 - kFieldBits)

/*****************************************************************
 * Render the view.  RADIX defines the radix point for coordinates
 * in the height and color fields.  Once the view is rendered, it
 * is directly copyed to the screen.
 ****************************************************************/
void UpdateScreen()
{
	long dx, dy;
	int col, angle;
	int i;
	int skyHoriz = gPitch + 20;	// overlap clipping of terrain
	UBYTE *d;

	// draw the gradient sky
	d = gRendBuffer;
	for ( i = 0; i < skyHoriz && i < kRendHeight; i++)
	{
		memset(d, 0xC2 + (skyHoriz - i) / 4, kRendWidth);
		d += kRendWidth;
	}

	for ( col = 0; col < kRendWidth; col++ )
	{
		angle = (kViewAngle * (kRendWidth - col * 2)) / kRendWidth;
		dx = COS(gYaw + angle) << (RADIX - 16);
		dy = SIN(gYaw + angle) << (RADIX - 16);

		// the -40 forces the view down to see more of the terrain
		CastRay(col, gPitch - 40, dx, dy);
	}

	// copy the buffer to the screen
	memcpy((char *)0xA0000, gRendBuffer, kRendWidth * kRendHeight);
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

	if (keyTest(KEY_LEFT))
		gYaw  = (gYaw + kYawAccel) & kAngleMask;
	if (keyTest(KEY_RIGHT))
		gYaw  = (gYaw - kYawAccel) & kAngleMask;

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
	ULONG startTime, stopTime;
	int fps10, frames = 0;
	int nOldMode;

	printf("Initializing math tables...\n");
	InitMathTables();

	printf("Loading terrain...\n");
	InitHeightField();

	keyInstall();		// install low-level keyboard handler

	nOldMode = getvmode();
	setvmode(0x13);
	gSetDACRange(0, 256, palette);

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

