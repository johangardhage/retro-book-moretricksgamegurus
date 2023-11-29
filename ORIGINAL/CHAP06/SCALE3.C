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
	int u, v, x, y;

	for ( x = x0; x < x1; x++ )
	{
		for ( y = y0; y < y1; y++ )
		{
			u = width * (x - x0) / (x1 - x0);
			v = height * (y - y0) / (y1 - y0);
			screen[x + y * 320] = bitmap[u + v * width];
		}
	}
}

