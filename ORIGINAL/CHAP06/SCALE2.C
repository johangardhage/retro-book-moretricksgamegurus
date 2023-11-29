#include <mem.h>
#include <conio.h>


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
UBYTE *screen = 0xA0000;
extern int width, height;


/*****************************************************************
 * ScaleBlit()
 *****************************************************************/
void cdecl ScaleBlit(UBYTE *bitmap, int x0, int y0,
	int x1, int y1)
{
	int i, j, x, y;

	memset(screen, 0, 64000);

	for ( i = 0; i < width; i++ )
	{
		for ( j = 0; j < height; j++ )
		{
			x = x0 + (x1 - x0) * i / width;
			y = y0 + (y1 - y0) * j / height;
			screen[x + y * 320] = bitmap[i + j * width];
		}
	}

	while ( !kbhit() );
	getch();
}



