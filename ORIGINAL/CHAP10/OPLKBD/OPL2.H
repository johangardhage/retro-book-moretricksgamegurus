/******************************************************************************
*File:			opl2.h
*Copyright: 1995 DiamondWare, Ltd.	All rights reserved.
*Written: 	Erik Lorenzen & Keith Weiner
*Purpose: 	Basic control functions for an opl2
*****************************************************************************/



/*
 . op1 is commonly known as the modulator
 . op2 is commonly known as the carrier
*/

typedef struct
{
	BYTE op1soundchar;
	BYTE op2soundchar;

	BYTE op1level_output;
	BYTE op2level_output;

	BYTE op1attack_decay;
	BYTE op2attack_decay;

	BYTE op1sustain_release;
	BYTE op2sustain_release;

	BYTE op1wave_select;
	BYTE op2wave_select;

	BYTE op1feedback_con;

	BYTE reserved1;
	BYTE reserved2;
	BYTE reserved3;
	BYTE reserved4;
	BYTE reserved5;

} opl2_INST;



int opl2_PTest(void);

void opl2_Reset(void);


void opl2_KeyOn(BYTE voice, opl2_INST *inst, BYTE fnum,
								BYTE keyon_block_fnum);

void opl2_KeyRelease(BYTE voice);
