/*****************************************************************************
*File:			oplkbd.c
*Copyright: 1995 DiamondWare, Ltd.	All rights reserved.
*Written: 	Erik Lorenzen & Keith Weiner
*Purpose: 	Implement simple polyphonic synthesizer using computer's keyboard
******************************************************************************/



#include <stdio.h>
#include <conio.h>



#include "sound.h"
#include "key.h"
#include "opl2.h"



//IBK stuff
#define NUMIBKINSTS  128
#define IBKDATASTART 4
#define LENINSTNAME  9


//Key stuff
#define NUMKEYS 	 12 									//number of keys being scanned for
																				//translation to "piano keys"

//FM stuff
#define NUMOCTAVES 8
#define NUMVOICES  9										//max number of voices that
																				//can play at one time
//Voice stuff
#define ERRVOICE 255



typedef struct
{
	BYTE scancode;
	BYTE playing;
	BYTE voice;
	BYTE lfnum;
	BYTE hfnum;
	char name[3];

} KEYNOTE;


//This table maps the a key(scancode) to its coresponding note
static KEYNOTE keynote[NUMKEYS] =
{ //SCANCODE	PLAYING  VOICE	LFNUM  HFNUM	NAME
	{key_Q, 		0,			 0, 		0x6b,  0x01,	"C#"},
	{key_A, 		0,			 0, 		0x81,  0x01,	"D "},
	{key_W, 		0,			 0, 		0x98,  0x01,	"D#"},
	{key_S, 		0,			 0, 		0xb0,  0x01,	"E "},
	{key_D, 		0,			 0, 		0xca,  0x01,	"F "},
	{key_R, 		0,			 0, 		0xe5,  0x01,	"F#"},
	{key_F, 		0,			 0, 		0x02,  0x02,	"G "},
	{key_T, 		0,			 0, 		0x20,  0x02,	"G#"},
	{key_G, 		0,			 0, 		0x41,  0x02,	"A "},
	{key_Y, 		0,			 0, 		0x63,  0x02,	"A#"},
	{key_H, 		0,			 0, 		0x87,  0x02,	"B "},
	{key_J, 		0,			 0, 		0xae,  0x02,	"C "}
};


static BYTE block[NUMOCTAVES] =
{
	0x00, 0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c
};


static opl2_INST inst[NUMIBKINSTS];
static char name[NUMIBKINSTS][LENINSTNAME];

static BYTE voicebusy[NUMVOICES];

static BYTE ginstnum;
static BYTE goctave;



static int LoadIBK(void)
{
	FILE *fp;

	if ((fp = fopen("gm1.ibk", "rb")) == NULL)
	{
		return (0);
	}

	fseek(fp, IBKDATASTART, SEEK_SET);				 //go to begining of real data

	fread(inst, 0x800, 1, fp);								 //load up inst parameters
	fread(name, 0x480, 1, fp);								 //load up inst names

	fclose(fp);

	return (1);
}


BYTE KeyOn(BYTE note)
{
	BYTE voice;
	BYTE fnum;
	BYTE keyon_block_fnum;

	for (voice=0;voice<NUMVOICES;voice++)
	{
		if (voicebusy[voice] == 0)
		{
			goto FoundVoice;
		}
	}

	/*
	 . This probably won't happen.  Most keyboards cannot recognize 9 keys
	 . pressed at once.
	*/
	return (ERRVOICE);

	FoundVoice:

	voicebusy[voice] = 1;

	fnum							= keynote[note].lfnum;
	keyon_block_fnum	= keynote[note].hfnum;
	keyon_block_fnum |= block[goctave]; 	//block is opl2-talk for "octave"

	opl2_KeyOn(voice, &inst[ginstnum], fnum, keyon_block_fnum);

	return (voice);
}


void KeyRelease(BYTE voice)
{
	voicebusy[voice] = 0; 								//deallocate the voice

	opl2_KeyRelease(voice);
}


void PianoKeys(void)
{
	BYTE x;

	for (x=0;x<NUMKEYS;x++) 							//go through scancodes for keys
	{
		if (key_IsDown(keynote[x].scancode) && !keynote[x].playing)
		{
			keynote[x].voice = KeyOn(x);

			if (keynote[x].voice != ERRVOICE)
			{
				keynote[x].playing = 1;

				printf("Octave %d  Note %s\n", goctave, keynote[x].name);
			}
			else
			{
				printf("All voices are in use\n");
			}
		}

		if (!key_IsDown(keynote[x].scancode) && keynote[x].playing)
		{
			keynote[x].playing = 0;
			KeyRelease(keynote[x].voice);
		}
	}
}


void ClearSound(void)
{
	BYTE voice;

	if (key_IsDown(key_SPACE))
	{
		key_DeRepeat(key_SPACE);

		for (voice=0;voice<NUMVOICES;voice++)
		{
			voicebusy[voice] = 0; 						//clear voice busy array
		}

		opl2_Reset(); 											//reset FM chip
	}
}


void OctaveChange(void)
{
	if (key_IsDown(key_RIGHT_ARROW))
	{
		key_DeRepeat(key_RIGHT_ARROW);

		if (++goctave < NUMOCTAVES) 				//max octave is 7
		{
			printf("Octave %d\n", goctave);
		}
		else
		{
			goctave--;
		}
	}

	if (key_IsDown(key_LEFT_ARROW))
	{
		key_DeRepeat(key_LEFT_ARROW);

		if (goctave--)
		{
			printf("Octave %d\n", goctave);
		}
		else
		{
			goctave++;
		}
	}
}


void InstrumentChange(void)
{
	if (key_IsDown(key_UP_ARROW))
	{
		key_DeRepeat(key_UP_ARROW);

		if (++ginstnum < NUMIBKINSTS) 			//max of 128 instruments in an .IBK
		{
			printf("Instrument # %3d, name %s\n", ginstnum, name[ginstnum]);
		}
		else
		{
			ginstnum--;
		}
	}

	if (key_IsDown(key_DOWN_ARROW))
	{
		key_DeRepeat(key_DOWN_ARROW);

		if (ginstnum--)
		{
			printf("Instrument # %3d, name %s\n", ginstnum, name[ginstnum]);
		}
		else
		{
			ginstnum++;
		}
	}
}


static void Play(void)
{
	while (!(key_IsDown(key_ESC)))				//run until user hits ESC
	{
		PianoKeys();												//play some notes (maybe)
		ClearSound(); 											//clear all notes (maybe)
		OctaveChange(); 										//change octave (maybe)
		InstrumentChange(); 								//change instrument (maybe)
	}
}


void main(void)
{
	BYTE x;

	printf("\nOPLKBD was written by Erik Lorenzed and Keith Weiner\n");
	printf("Copyright 1995 DiamondWare, Ltd.  All rights reserved.\n\n\n");
	printf("LEFT & RIGHT ARROW change octaves\n");
	printf("UP & DOWN ARROW    change instruments\n");
	printf("SPACE              to clear sounds\n");
	printf("ESC                to exit\n\n");

	for (x=0;x<NUMVOICES;x++) 						//init voicebusy[]
	{
		voicebusy[x] = 0;
	}

	ginstnum = 0; 												//start at reasonable values
	goctave  = 3;

	if (opl2_PTest()) 										//test for presence of opl2
	{
		if (LoadIBK())
		{
			opl2_Reset(); 										//clear it for playback
			key_Init(); 											//start keyboard handler

			Play();

			key_Kill(); 											//shutdown keyboard handler
			opl2_Reset(); 										//stop all playing notes
		}
		else
		{
			printf("Couldn't find GM1.IBK :(\n");
		}
	}
	else
	{
		printf("Sorry, no FM found :(\n");
	}
}
