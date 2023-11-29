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
	int x, y;
	long u, v, du, dv;
	UBYTE *destRow, *dest, *sourceRow;

	du = (width << 16) / (x1 - x0);
	dv = (height << 16) / (y1 - y0);

	v = 0;
	destRow = screen + 320 * y0 + x0;

	for ( y = y0; y < y1; y++ )
	{
		u = 0;
		sourceRow = bitmap + (v >> 16) * width;
		dest = destRow;

		for ( x = x0; x < x1; x++ )
		{
			*dest++ = sourceRow[u >> 16];
			u += du;
		}
		v += dv;
		destRow += 320;
	}
}


