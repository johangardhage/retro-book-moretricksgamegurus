/****************************************************************************
*File:			key.c
*Copyright: 1995 DiamondWare, Ltd.	All rights reserved.
*Written: 	Erik Lorenzen & Keith Weiner
*Purpose: 	Basic keyboard handler
*****************************************************************************/



#pragma check_stack(off)



#include <conio.h>
#include <dos.h>


#include "sound.h"
#include "key.h"



#define KBDINT	(WORD) 0x09

#define EOI 		0x20
#define PICPORT 0x20
#define IMRPORT 0x21
#define KBDATA	0x60

#define SPECIAL 0x60

#define NUMKEYS 128



static void (_interrupt _far *oldisr)();
static volatile BYTE isdown[NUMKEYS];



static void _interrupt _far NewISR(void)
{
	int result;

	result = inp(KBDATA); 								//read the scancode from keyboard

	if ((result & 0x7f) != SPECIAL) 			//if its a special code ignore it
	{
		if (result & 0x80)									//if high bit is set, it's a key Break
		{
			isdown[result & 0x7f]  = 0;
		}
		else																//otherwise its a key Make
		{
			isdown[result & 0x7f]  = 1;
		}
	}

	outp(PICPORT, EOI); 									//send End Of Intterupt to PIC
}


void key_DeRepeat(BYTE key)
{
	while(isdown[key])
	{
		//Wait
	}
}


BYTE key_IsDown(BYTE key)
{
	return(isdown[key]);
}


void key_Init(void)
{
	WORD x;

	for (x=0;x<NUMKEYS;x++) 							//reset isdown[]
	{
		isdown[x] = 0;
	}

	oldisr = _dos_getvect(KBDINT);				//save the old keyboard IRQ vector
	_dos_setvect(KBDINT, NewISR); 				//hook our ISR
}


void key_Kill(void)
{
	_dos_setvect(KBDINT, oldisr); 				//unhook our ISR, restoring old one
}
