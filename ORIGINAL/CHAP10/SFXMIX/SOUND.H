/******************************************************************************
*File:			sound.h
*Copyright: 1995 DiamondWare, Ltd.	All rights reserved.
*Written: 	Erik Lorenzen & Keith Weiner
*Purpose: 	Useful declarations
******************************************************************************/



#ifndef BYTE
	#define BYTE unsigned char
#endif

#ifndef WORD
	#define WORD unsigned short
#endif

#ifndef DWORD
	#define DWORD unsigned long
#endif

#ifndef INT8
	#define INT8 signed char
#endif

#ifndef INT16
	#define INT16 signed short
#endif

#ifndef INT32
	#define INT32 signed long
#endif



typedef struct
{
	INT8	*samples;
	DWORD  length;

} SOUND;
