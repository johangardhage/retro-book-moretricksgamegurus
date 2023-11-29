/******************************************************************************
File: 		 findsb.c
Tab stops: every 2 collumns
Project:	 FINDSB utility
Copyright: 1994 DiamondWare, Ltd.  All rights reserved.
Written:	 Keith Weiner & Erik Lorenzen
Purpose:	 Example code to autodetect and print out the sound hardware
History:	 KW 10/21/94 Started
					 KW/EL 02/01/95 Finalized
******************************************************************************/



#include <stdio.h>
#include <stdlib.h>

#include "dws.h"



static void DisplayError(word errornum)
{
	switch (errornum)
	{
		/*
		 . Note that only those errors which could occur when calling
		 . dws_DetectHardWare are checked for.
		*/
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
		default:
		{
			/*
			 . This should never occur and probably won't affect sound
			 . play(?).  If it happens please contact DiamondWare.
			*/

			printf("I'm confused!  Where am I?  HOW DID I GET HERE????\n");
			printf("The ERROR number is: %d\n",errornum);
		}
	}
}


void main(void)
{
	dws_DETECTOVERRIDES dov;
	dws_DETECTRESULTS 	dres;

	printf("\nFINDSB is Copyright 1994 DiamondWare, Ltd.\nAll rights reserved.\n\n\n");

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

	if (dres.capability & (dws_capability_FM | dws_capability_DIG))
	{
		printf("Base port is %x hex.\n\n", dres.baseport);

		if (dres.mixtyp > 1)
		{
			printf("The sound hardware supports mixing.\n\n");
		}
		else
		{
			printf("The sound hardware does not support mixing.\n\n");
		}

		if (dres.capability & dws_capability_FM)
		{
			printf("The sound hardware supports FM music playback.\n\n");
		}

		if (dres.capability & dws_capability_DIG)
		{
			printf("The sound hardware supports digitized sound playback.\n");

			printf("The sound hardware uses DMA channel %d and IRQ level %d.\n\n",
						 dres.digdma, dres.digirq);
		}
	}
	else
	{
		printf(" No sound hardware detected.\n");
	}
}
