'******************************************************************************
'File:      findsb.bas
'Tab stops: every 2 collumns
'Project:   FINDSB utility
'Copyright: 1994 DiamondWare, Ltd.  All rights reserved.
'Written:   Erik Lorenzen & Don Lemons
'Purpose:   Example code to autodetect and print out the sound hardware
'History:   KW 10/21/94 Started findsb.c
'           DL 11/12/94 translated to BASIC
'           EL 02/02/95 Cleaned up & Finalized
'******************************************************************************



'$INCLUDE: 'dws.bi'



'DECLARE VARIABLES
	COMMON SHARED dov 	AS dwsDETECTOVERRIDES
	COMMON SHARED dres	AS dwsDETECTRESULTS
	COMMON SHARED ideal AS dwsIDEAL



SUB DisplayError(errornum)
	SELECT CASE errornum

		'Note that only those errors which could occur when calling
		'dws_DetectHardWare are checked for.

		CASE dwsALREADYINITTED
			'If we get here, it means you've called dws_Init() already.  Calling
			'dws_DetectHardWare() at this point would cause zillions of
			'problems if we let the call through.
			PRINT "The STK was already initialized"

		CASE dwsDetectHardwareUNSTABLESYSTEM
			'Please report it to DiamondWare if you get here!
			'
			'Ideally, you would disable control-C here, so that the user can't
			'hit control-alt-delete, causing SmartDrive to flush its (possibly
			'currupt) buffers.
			PRINT "The system is unstable!"
			PRINT "Please power down now!"

			AGAIN:
			GOTO AGAIN

		'The following three errors are USER/PROGRAMMER errors.  You forgot
		'to fill the cardtyp struct full of -1's (except in those fields
		'you intended to override, or the user (upon the unlikly event that
		'the STK was unable to find a card) gave you a bad overide value.

		CASE dwsDetectHardwareBADBASEPORT
			'You set dov.baseport to a bad value, or
			'didn't fill it with a -1.
			PRINT "Bad port address"

		CASE dwsDetectHardwareBADDMA
			'You set dov.digdma to a bad value, or
			'didn't fill it with a -1.
			PRINT "Bad DMA channel"

		CASE dwsDetectHardwareBADIRQ
			'You set dov.digirq to a bad value, or
			'didn't fill it with a -1.
			PRINT "Bad IRQ level"

		CASE ELSE
			'This should never occur and probably will not affect sound
			'play(?). If it happens please contact DiamondWare.
			PRINT "I'm confused!  Where am I?  HOW DID I GET HERE????"
			PRINT "The ERROR number is:";errornum

	END SELECT

END SUB



'START OF MAIN

	PRINT
	PRINT "FINDSB is Copyright 1994, DiamondWare, Ltd."
	PRINT "All rights reserved."
	PRINT : PRINT : PRINT

	'We need to set every field to -1 in dws_DETECTOVERRIDES struct; this
	'tells the STK to autodetect everything.  Any other value
	'overrides the autodetect routine, and will be accepted on
	'faith, though the STK will verify it if possible.

	dov.baseport = -1
	dov.digdma	 = -1
	dov.digirq	 = -1

	IF DWSDetectHardWare(dov, dres) = 0 THEN
		errnum = dwsErrNo
		DisplayError(errnum)
	END IF

	IF (dres.capability AND dwscapabilityFM) = dwscapabilityFM THEN

		PRINT "Base port is ";HEX$(dres.baseport);" hex."
		PRINT

		IF dres.mixtyp = 1 THEN
			PRINT "Mixing will be done in software."
			PRINT
		ELSE
			PRINT "The sound hardware supports mixing."
			PRINT
		ENDIF

		IF (dres.capability AND dwscapabilityFM) = dwscapabilityFM THEN
			PRINT "The sound hardware supports FM music playback."
			PRINT
		END IF

		IF (dres.capability AND dwscapabilityDIG) = dwscapabilityDIG THEN
			PRINT "The sound hardware supports digitized sound playback."
			PRINT "The sound hardware uses DMA channel";dres.digdma;"and IRQ level";dres.digirq;"."
			PRINT
		END IF

	ELSE
		PRINT "No sound hardware detected."
		PRINT
	ENDIF
END
