/*****************************************************************
 * Defines
 ****************************************************************/
#define kBitmapWidth	32
#define kScreenWidth 	320
#define kScreenHeight	200
#define kScreenCenterX	(kScreenWidth / 2)
#define kScreenCenterY	(kScreenHeight / 2)
#define kAngle360		256
#define kAngleMask		(kAngle360 - 1)
#define kAngle180		(kAngle360 / 2)
#define kAngle90		(kAngle360 / 4)


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
long aspectAdjust = (6 << 16) / 5;
extern long sinTable[kAngle360];


/*****************************************************************
 * Inline Functions
 *****************************************************************/
long Mul16_16( long a, long b );
#pragma aux Mul16_16 =\
	"imul	edx",\
	"shrd	eax,edx,16"\
	parm [eax] [edx]


/*****************************************************************
 * RotateBlit()
 *****************************************************************/
void cdecl RotateBlit(UBYTE *bitmap, int angle, long scale)
{
	UBYTE *dest;
	long u, v, rowU, rowV, startingU, startingV;
	long duCol, dvCol, duRow, dvRow;
	int x, y;

	// center of 32x32 bitmap
	startingU = 16 << 16;
	startingV = 16 << 16;

    // calculate deltas
    duCol = sinTable[(angle + kAngle90) & kAngleMask];
    dvCol = sinTable[angle];

	duCol = Mul16_16(duCol, scale);
	dvCol = Mul16_16(dvCol, scale);

    duRow = Mul16_16(-dvCol, aspectAdjust);
    dvRow = Mul16_16(duCol, aspectAdjust);

	startingU -= kScreenCenterX * duCol + kScreenCenterY * duRow;
	startingV -= kScreenCenterX * dvCol + kScreenCenterY * dvRow;

    rowU = startingU;
    rowV = startingV;
	dest = screen;

    for ( y = 0; y < 200; y++)
    {
        u = rowU;
        v = rowV;

        for ( x = 0; x < 320; x++)
        {
            *dest++ = bitmap[((u >> 16) & 31) +
				((v >> 16) & 31) * 32];
            u += duCol;
            v += dvCol;
        }

        rowU += duRow;
        rowV += dvRow;
    }
}



