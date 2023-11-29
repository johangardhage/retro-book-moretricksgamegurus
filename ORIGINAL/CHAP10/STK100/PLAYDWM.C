/******************************************************************************
File: 		 playdwm.c
Tab stops: every 2 collumns
Project:	 DWM Player
Copyright: 1994 DiamondWare, Ltd.  All rights reserved.*
Written:	 Keith Weiner & Erik Lorenzen
Purpose:	 Contains simple example code to show how to load/play a .DWM file
History:	 KW 10/21/94 Started
					 EL 02/21/95 Finalized

Notes
-----

The bulk of this file is error checking logic.

However, this code isn't really robust when it comes to standard error checking
and particularly recovery, software engineering technique, etc.  A buffer
is statically allocated.	A better technique would be to use fstat() or stat()
to determine the file's size then malloc(size).  The STK will handle songs
larger than 64K (but not digitized sounds).  Obviously, you'd need to fread()
such a file in chunks, or write some sort of hfread() (huge fread).  Also,
exitting and cleanup is not handled robustly in this code.	The code below can
only be validated by extremely careful scrutiny to make sure each case is
handled properly.  A better method would the use of C's atexit function.

But all such code would make this example file less clear; its purpose was
to illustrate how to call the STK, not how to write QA-proof software.


*Permission is expressely granted to use DisplayError or any derivitive made
 from it to registered users of the STK.
******************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "dws.h"
#include "dwt.h"


#define BUFFSIZE 65535		//If the linker outputs the error
													//"stack plus data exceed 64K"
													//try reducing BUFFSIZE to about 32K,
													//or compile for large model.
byte song[BUFFSIZE];



static void DisplayError(word errornum)
{
	switch (errornum)
	{
		case dws_EZERO:
		{
			/*
			 . This should not have happened, considering how we got here!
			*/
			printf("I'm confused!  Where am I?  HOW DID I GET HERE????\n");
			printf("The ERROR number is: %d\n",errornum);

			break;
		}
		case dws_NOTINITTED:
		{
			/*
			 . If we get here, it means you haven't called dws_Init().
			 . The STK needs to initialize itself and the hardware before
			 . it can do anything.
			*/
			printf("The STK was not initialized\n");

			break;
		}
		case dws_ALREADYINITTED:
		{
			/*
			 . If we get here, it means you've called dws_Init() already.  Calling
			 . dws_DetectHardWare() at this point would cause zillions of
			 . problems if we let the call through.
			*/
			printf("The STK was already initialized\n");

			break;
		}
		case dws_NOTSUPPORTED:
		{
			/*
			 . If we get here, it means that either the user's machine does not
			 . support the function you just called, or the STK was told not to
			 . support it in dws_Init.
			*/
			printf("Function not supported\n");

			break;
		}
		case dws_DetectHardware_UNSTABLESYSTEM:
		{
			/*
			 . Please report it to DiamondWare if you get here!
			 .
			 . Ideally, you would disable control-C here, so that the user can't
			 . hit control-alt-delete, causing SmartDrive to flush its (possibly
			 . currupt) buffers.
			*/
			printf("The system is unstable!\n");
			printf("Please power down now!");

			#define ever ;;
			for (ever);
		}

		/*
		 . The following three errors are USER/PROGRAMMER errors.  You forgot
		 . to fill the cardtyp struct full of -1's (except in those fields
		 . you intended to override, or the user (upon the unlikly event that
		 . the STK was unable to find a card) gave you a bad overide value.
		*/
		case dws_DetectHardware_BADBASEPORT:
		{
			/*
			 . You set dov.baseport to a bad value, or
			 . didn't fill it with a -1.
			*/
			printf("Bad port address\n");

			break;
		}
		case dws_DetectHardware_BADDMA:
		{
			/*
			 . You set dov.digdma to a bad value, or
			 . didn't fill it with a -1.
			*/
			printf("Bad DMA channel\n");

			break;
		}
		case dws_DetectHardware_BADIRQ:
		{
			/*
			 . You set dov.digirq to a bad value, or
			 . didn't fill it with a -1.
			*/
			printf("Bad IRQ level\n");

			break;
		}
		case dws_Kill_CANTUNHOOKISR:
		{
			/*
			 . The STK points the interrupt vector for the sound card's IRQ
			 . to its own code in dws_Init.
			 .
			 . dws_Kill was unable to restore the vector to its original
			 . value because other code has hooked it after the STK
			 . initialized(!)  This is really bad.	Make the user get rid
			 . of it and call dws_Kill again.
			*/
			printf("Get rid of your TSR, pal!\n");
			printf("(Press any key)\n");
			getch();

			break;
		}
		case dws_X_BADINPUT:
		{
			/*
			 . The mixer funtions can only accept volumes between 0 & 255,
			 . the volume will remain unchanged.
			*/
			printf("Bad mixer level\n");

			break;
		}
		case dws_D_NOTADWD:
		{
			/* You passed the STK a pointer to something which is not a .DWD file! */
			printf("The file you are attempting to play is not a .DWD\n");

			break;
		}
		case dws_D_NOTSUPPORTEDVER:
		{
			/*
			 . The STK can't play a .DWD converted using a version of VOC2DWD.EXE
			 . newer than itself.  And, although we'll try to maintain backwards
			 . compatibility, we may not be able to guarantee that newer versions
			 . of the code will be able to play older .DWD files.  In any event,
			 . it's a good idea to always convert .VOC files with the utility
			 . which comes with the library you're linking into your application.
			*/
			printf("Please reconvert this file using the VOC2DWD.EXE which came with this library");

			break;
		}
		case dws_D_INTERNALERROR:
		{
			/*
			 . This error should never occur and probably will not affect sound
			 . play(?).  If it happens please contact DiamondWare.
			*/
			printf("An internal error has occured\nPlease contact DiamondWare\n");

			break;
		}
		case dws_DPlay_NOSPACEFORSOUND:
		{
			/*
			 . This error is more like a warning, though it may happen on a
			 . regular basis, depending on how many sounds you told the STK
			 . to allow in dws_Init, how you chose to prioritize sounds and
			 . how many sounds are currently being played.
			*/
			printf("No more room for new digitized sounds right now\n");

			break;
		}
		case dws_DSetRate_FREQTOLOW:
		{
			/*
			 . The STK will set rate as close as possible to the indicated rate
			 . but cannot set a rate that low.
			*/
			printf("Playback frequency too low\n");

			break;
		}
		case dws_DSetRate_FREQTOHIGH:
		{
			/*
			 . The STK will set rate as close as possible to the indicated rate
			 . but cannot set a rate that high.
			*/
			printf("Playback frequency too high\n");

			break;
		}
		case dws_MPlay_NOTADWM:
		{
			/*
			 . You passed the STK a pointer to something which is not a .DWM file!
			*/
			printf("The file you are attempting to play is not a .DWM\n");

			break;
		}
		case dws_MPlay_NOTSUPPORTEDVER:
		{
			/*
			 . The STK can't play a .DWM converted using a version of VOC2DWM.EXE
			 . newer than itself.  And, although we'll try to maintain backwards
			 . compatibility, we may not be able to guarantee that newer versions
			 . of the code will be able to play older .DWM files.  In any event,
			 . it's a good idea to always convert .MID files with the utility
			 . which comes with the library you're linking into your application.
			*/
			printf("Please reconvert this file using the MID2DWM.EXE which came with this library");

			break;
		}
		case dws_MPlay_INTERNALERROR:
		{
			/*
			 . This error should never occur and probably will not affect sound
			 . play(?).  If it happens please contact DiamondWare.
			*/
			printf("An internal error has occured\nPlease contact DiamondWare\n");

			break;
		}
		default:
		{
			/*
			 . This should never occur and probably will not affect sound
			 . play(?). If it happens please contact DiamondWare.
			*/
			printf("I'm confused!  Where am I?  HOW DID I GET HERE????\n");
			printf("The ERROR number is: %d\n",errornum);
		}
	}
}


void main(int argc, char **argv)
{
	FILE								*fp;
	dws_DETECTOVERRIDES dov;
	dws_DETECTRESULTS 	dres;
	dws_IDEAL 					ideal;
	dws_MPLAY 					mplay;
	int 								input=0;
	word								musvol=255; 	//Default mxr volume at startup is max
	word								songstatus;

	printf("\nPLAYDWM is Copyright 1994 DiamondWare, Ltd.\nAll rights reserved.\n\n\n");

	if (argc < 2)
	{
		printf("Usage PLAYDWM <dwm-file>\n");
		exit(-1);
	}

	fp = fopen(argv[1], "rb");

	if (fp == NULL)
	{
		printf("Unable to open %s\n", argv[1]);
		exit(-1);
	}

	fread(song, (size_t)BUFFSIZE, 1, fp);  //if file's len < BUFFSIZE, this works

	fclose(fp);

	/*
	 . We need to set every field to -1 in dws_DETECTOVERRIDES struct; this
	 . tells the STK to autodetect everything.	Any other value
	 . overrides the autodetect routine, and will be accepted on
	 . faith, though the STK will verify it if possible.
	*/
	dov.baseport = (word)-1;
	dov.digdma	 = (word)-1;
	dov.digirq	 = (word)-1;

	if (!dws_DetectHardWare(&dov, &dres))
	{
		DisplayError(dws_ErrNo());
		exit(-1);
	}

	if (!(dres.capability & dws_capability_FM))
	{
		printf("FM support not found\n");
		exit(-1);
	}

	/*
	 . The "ideal" struct tells the STK how you'd like it to initialize the
	 . sound hardware.	In all cases, if the hardware won't support your
	 . request, the STK will go as close as possible.  For example, not all
	 . sound boards will support al sampling rates (some only support 5 or
	 . 6 discrete rates).
	*/
	ideal.musictyp	 = 1; 			//for now, it's OPL2 music
	ideal.digtyp		 = 0; 			//0=No Dig, 8=8bit, 16=16bit
	ideal.digrate 	 = 0; 			//sampling rate, in Hz
	ideal.dignvoices = 0; 			//number of voices (up to 16)
	ideal.dignchan	 = 0; 			//1=mono, 2=stereo

	if (!dws_Init(&dres, &ideal))
	{
		DisplayError(dws_ErrNo());
		exit(-1);
	}

	/*
	 . 72.8Hz is a decent compromise.  It will work in a Windows DOS box
	 . without any problems, and yet it allows music to sound pretty good.
	 . In my opinion, there's no reason to go lower than 72.8 (unless you
	 . don't want the hardware timer reprogrammed)--music sounds kinda chunky
	 . at lower rates.	You can go to 145.6 Hz, and get smoother (very
	 . subtly) sounding music, at the cost that it will NOT run at the correct
	 . (or constant) speed in a Windows DOS box.
	*/
	dwt_Init(dwt_72_8HZ);

	/*
	 . Set Music Volume to about 4/5 th's of max
	*/
	musvol = 200;
	if (!dws_XMusic(musvol))
	{
		DisplayError(dws_ErrNo());
	}

	mplay.track = song;
	mplay.count = 1;						//0=infinite loop, 1-N num times to play sound

	if (!dws_MPlay(&mplay))
	{
		DisplayError(dws_ErrNo());
		goto KillIt;
	}

	/*
	 . We're playing.  Let's exit when the song is over, and allow the user
	 . to fiddle with the volume level (mixer) in the meantime
	*/

	if (!dws_MSongStatus(&songstatus))
	{
		DisplayError(dws_ErrNo());
		goto KillIt;
	}

	printf("Press + or - to change playback volume \n");

	while (songstatus)
	{
		if (kbhit())
		{
			input = getch();
		}
		else
		{
			input = 0;
		}

		switch (input)
		{
			case 'q':
			case 'Q':
			case 27:								//ESC
			{
				if (!dws_MClear())
				{
					DisplayError(dws_ErrNo());
				}

				break;
			}
			case '+':
			{
				musvol++;

				printf("Music Volume is %d\n", musvol);

				if (!dws_XMusic(musvol))
				{
					DisplayError(dws_ErrNo());
				}

				break;
			}
			case '-':
			{
				musvol--;

				printf("Music Volume is %d\n", musvol);

				if (!dws_XMusic(musvol))
				{
					DisplayError(dws_ErrNo());
				}

				break;
			}
		}

		if (!dws_MSongStatus(&songstatus))
		{
			DisplayError(dws_ErrNo());
			goto KillIt;
		}
	}

	KillIt:

	dwt_Kill();

	KillIt2:

	if (!dws_Kill())
	{
		/*
		 . If an error occurs here, it's either dws_Kill_CANTUNHOOKISR
		 . or dws_NOTINITTED.  If it's dws_Kill_CANTUNHOOKISR the user
		 . must remove his tsr, and dws_Kill must be called again.	If it's
		 . dws_NOTINITTED, there's nothing to worry about at this point.
		*/
		DisplayError(dws_ErrNo());

		if (dws_ErrNo() ==	dws_Kill_CANTUNHOOKISR)
		{
			goto KillIt2;
		}
	}
}
