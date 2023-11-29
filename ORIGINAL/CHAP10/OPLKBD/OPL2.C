/****************************************************************************
*File:			opl2.c
*Copyright: 1995 DiamondWare, Ltd.	All rights reserved.
*Written: 	Erik Lorenzen & Keith Weiner
*Purpose: 	Basic control functions for an opl2 in melody mode
*****************************************************************************/



#include <stdio.h>
#include <conio.h>


#include "sound.h"
#include "opl2.h"



#define STATUS						 0x388			//read-only

#define RSEL							 0x388			//write-only
#define DATA							 0x389			//write-only

#define SHORT 						 7					//aprox 3.3 æsec delay
#define LONG							 36 				//aprox 23 æsec delay
#define TIMEOUT 					 144				//aprox 92 æsec delay

#define MAXVOICES 				 9					//9 2op voices

#define TEST							 0x01 			//one per chip
#define TIMER1						 0x02 			//one per chip	80	æsec resolution
#define TIMER2						 0x03 			//one per chip	320 æsec resolution
#define IRQ_TIMER_CNTRL 	 0x04 			//one per chip
#define CSM_SEL 					 0x08 			//one per chip

#define SOUNDCHAR 				 0x20 			//one per operator
#define LEVEL_OUTPUT			 0x40 			//one per operator
#define ATTACK_DECAY			 0x60 			//one per operator
#define SUSTAIN_RELEASE 	 0x80 			//one per operator
#define FNUM							 0xa0 			//one per operator pair
#define KEYON_BLOCK_FNUM	 0xb0 			//one per operator pair
#define DRUMCONTROLL			 0xbd 			//one per chip
#define FEEDBACK_CON			 0xc0 			//one per operator pair
#define WAVE_SELECT 			 0xe0 			//one per operator

#define RESET 						 0x60 			//reset command
#define ENABLE_IRQ				 0x80 			//enable timer IRQ command
#define STARTTIMER1 			 0x21 			//start timer 1 command
#define WAVESEL 					 0x20 			//enable wave select command

#define KEYON 						 0x20 			//key on mask



//VOICE 							 #1 	 #2 	 #3 	 #4 	 #5 	 #6 	 #7 	 #8 	 #9
static op1_port[9] = {0x00, 0x01, 0x02, 0x08, 0x09, 0x0a, 0x10, 0x11, 0x12};
static op2_port[9] = {0x03, 0x04, 0x05, 0x0b, 0x0c, 0x0d, 0x13, 0x14, 0x15};

static BYTE kbf[9];



static void Delay(unsigned numreads)
{
	while (numreads--)
	{
		inp(STATUS);								 //read the opl2 status port in order
	} 														 //to induce a timed delay
}


static void Write(unsigned port, unsigned value)
{
	outp(RSEL, port); 						 //select register to write to
	Delay(SHORT);

	outp(DATA, value);						 //write value to register
	Delay(LONG);
}


int opl2_PTest(void)
{
	int result1, result2;

	Write(IRQ_TIMER_CNTRL, RESET);			 //reset timer
	Write(IRQ_TIMER_CNTRL, ENABLE_IRQ);  //enable timer IRQ
	result1 = inp(STATUS);							 //get result after reset/before timeout

	Write(TIMER1, 0xff);								 //write low value to timer1
	Write(IRQ_TIMER_CNTRL, STARTTIMER1); //write timer1 start mask
	Delay(TIMEOUT); 										 //wait long enough for timer to expire
	result2 = inp(STATUS);							 //get result after a timeout

	Write(IRQ_TIMER_CNTRL, RESET); //reset timer

	if ((result1 &= 0xe0) != 0) 	 //MASK bit for timer gets set if it times out
	{
		return(0);									 //fail if set
	}

	if ((result2 &= 0xe0) != 0xc0) //make sure MASK bit is set to "timed out"
	{
		return(0);									 //fail if not set
	}

	return(1);										 //pass, it would appear that the
} 															 //timer is present


void opl2_Reset(void)
{
	BYTE voice;

	Write(TIMER1, 			 WAVESEL); //enable wave select
	Write(IRQ_TIMER_CNTRL, RESET); //reset timer (keep masked)
	Write(CSM_SEL,				 0x00);  //clear Composite Speech Mode
	Write(DRUMCONTROLL, 	 0x00);  //set melody mode

	for (voice=0;voice<MAXVOICES;voice++)
	{
		Write(LEVEL_OUTPUT + op1_port[voice], 0xff);		//attenuate note volume
		Write(LEVEL_OUTPUT + op2_port[voice], 0xff);

		Write(ATTACK_DECAY + op1_port[voice], 0xff);		//shut Note down
		Write(ATTACK_DECAY + op2_port[voice], 0xff);

		Write(SUSTAIN_RELEASE + op1_port[voice], 0x0f); //shut Note down
		Write(SUSTAIN_RELEASE + op2_port[voice], 0x0f);

		Write(KEYON_BLOCK_FNUM + voice, 0x00);					//key Off all notes
	}
}


void opl2_KeyRelease(BYTE voice)
{
	Write(KEYON_BLOCK_FNUM + voice, kbf[voice] & 0xdF);
}


void opl2_KeyOn(BYTE voice, opl2_INST *inst, BYTE fnum,
								BYTE keyon_block_fnum)
{
	//key voice off before modification
	Write(LEVEL_OUTPUT		 + op1_port[voice], 0xff);
	Write(LEVEL_OUTPUT		 + op2_port[voice], 0xff);
	Write(KEYON_BLOCK_FNUM + voice					, 0x00);

	Write(SOUNDCHAR 			+ op1_port[voice], inst->op1soundchar 		 );
	Write(SOUNDCHAR 			+ op2_port[voice], inst->op2soundchar 		 );
	Write(LEVEL_OUTPUT		+ op1_port[voice], inst->op1level_output	 );
	Write(LEVEL_OUTPUT		+ op2_port[voice], inst->op2level_output	 );
	Write(ATTACK_DECAY		+ op1_port[voice], inst->op1attack_decay	 );
	Write(ATTACK_DECAY		+ op2_port[voice], inst->op2attack_decay	 );
	Write(SUSTAIN_RELEASE + op1_port[voice], inst->op1sustain_release);
	Write(SUSTAIN_RELEASE + op2_port[voice], inst->op2sustain_release);
	Write(WAVE_SELECT 		+ op1_port[voice], inst->op1wave_select 	 );
	Write(WAVE_SELECT 		+ op2_port[voice], inst->op2wave_select 	 );
	Write(FEEDBACK_CON		+ voice 				 , inst->op1feedback_con	 );
	Write(FNUM						+ voice 				 , fnum 									 );

	//we leave this for last, or else you will hear the regs being programmed
	kbf[voice] = (BYTE)(keyon_block_fnum | KEYON);
	Write(KEYON_BLOCK_FNUM + voice, kbf[voice]);
}
