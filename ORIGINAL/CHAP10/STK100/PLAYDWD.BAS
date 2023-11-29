'******************************************************************************
'FILE:      playdwd.bas
'Tab stops: every 2 collumns
'Project:   DWD Player
'Copyright: 1994 DiamondWare, Ltd.  All rights reserved.*
'Written:   Erik Lorenzen & Don Lemmons
'Purpose:   Contains simple example code to show how to load/play a .DWD file
'History:   KW 10/21/94 Started playdwd.c
'           DL 11/12/94 Translated to BASIC
'           EL 01/12/95 Cleaned up & Finalized
'
'Notes
'-----
'
'The bulk of this file is error checking logic.
'
'However, this code isn't really robust when it comes to standard error checking
'and particularly recovery, software engineering technique, etc.  A buffer of
'size 32767 is statically allocated.  A better technique would be to
'determine the file's size.  The STK will handle songs larger than 64K
'(but not digitized sounds). Also, exitting and cleanup is not handled
'robustly in this code.  The code below can only be validated by
'extremely careful scrutiny to make sure each case is handled properly.
'
'But all such code would make this example file less clear; its purpose was
'to illustrate how to call the STK, not how to write QA-proof software.
'
'
'*Permission is expressely granted to use DisplayError or any derivitive made
' from it to registered users of the STK.
'******************************************************************************/



'$INCLUDE: 'dws.bi'



TYPE BUFFTYP
	buf AS STRING * 32767
END TYPE



'DECLARE VARIABLES
	COMMON SHARED dov 	AS dwsDETECTOVERRIDES
	COMMON SHARED dres	AS dwsDETECTRESULTS
	COMMON SHARED ideal AS dwsIDEAL
	COMMON SHARED dplay AS dwsDPLAY



DIM SHARED buffer(0) AS BUFFTYP 'set aside string area for song to load into
																'by doing it this way we give QBasic the
																'opportunity to place the song into far mem



SUB DisplayError(errornum)
	SELECT CASE errornum

		CASE dwsEZERO
			'This should not have happened, considering how we got here!
			PRINT"I'm confused!  Where am I?  HOW DID I GET HERE????"
			PRINT "The ERROR number is:";errornum

		CASE dwsNOTINITTED
			'If we get here, it means you haven't called dwsInit().
			'The STK needs to initialize itself and the hardware before
			'it can do anything.
			PRINT"The STK was not initialized"

		CASE dwsALREADYINITTED
			'If we get here, it means you've called dwsInit() already.	Calling
			'dwsDetectHardWare() at this point would cause zillions of
			'problems if we let the call through.
			PRINT"The STK was already initialized"

		CASE dwsNOTSUPPORTED:
			'If we get here, it means that either the user's machine does not
			'support the function you just called, or the STK was told not to
			'support it in dwsInit.
			PRINT"Function not supported"

		CASE dwsDetectHardwareUNSTABLESYSTEM
			' Please report it to DiamondWare if you get here!
			'
			' Ideally, you would disable control-C here, so that the user can't
			' hit control-alt-delete, causing SmartDrive to flush its (possibly
			' currupt) buffers.
			PRINT"The system is unstable!"
			PRINT"Please power down now!"

			AGAIN:
			GOTO AGAIN

		'The following three errors are USER/PROGRAMMER errors.  You forgot
		'to fill the cardtyp struct full of -1's (except in those fields
		'you intended to override, or the user (upon the unlikly event that
		'the STK was unable to find a card) gave you a bad overide value.

		CASE dwsDetectHardwareBADBASEPORT
			'You set dov.baseport to a bad value, or
			'didn't fill it with a -1.
			PRINT"Bad port address"

		CASE dwsDetectHardwareBADDMA
			'You set dov.digdma to a bad value, or
			'didn't fill it with a -1.
			PRINT"Bad DMA channel"

		CASE dwsDetectHardwareBADIRQ
			'You set dov.digirq to a bad value, or
			'didn't fill it with a -1.
			PRINT"Bad IRQ level"

		CASE dwsKillCANTUNHOOKISR
			'The STK points the interrupt vector for the sound card's IRQ
			'to its own code in dws_Init.
			'
			'dws_Kill was unable to restore the vector to its original
			'value because other code has hooked it after the STK
			'initialized(!)  This is really bad.  Make the user get rid
			'of it and call dws_Kill again.

			PRINT"Get rid of your TSR, pal!"
			INPUT"(Hit ENTER when ready)";g$

		CASE dwsXBADINPUT
			'The mixer funtion's can only accept volumes between 0 & 255,
			'the volume will remain unchanged.

			PRINT"Bad mixer level"

		CASE dwsDNOTADWD
			'You passed the STK a pointer to something which is not a .DWD file!
			PRINT"The file you are attempting to play is not a .DWD"

		CASE dwsDNOTSUPPORTEDVER
			'The STK can't play a .DWD converted using a version of VOC2DWD.EXE
			'newer than itself.  And, although we'll try to maintain backwards
			'compatibility, we may not be able to guarantee that newer versions
			'of the code will be able to play older .DWD files.  In any event,
			'it's a good idea to always convert .VOC files with the utility
			'which comes with the library you're linking into your application.
			PRINT"Please reconvert this file using the VOC2DWD program which came with this library"

		CASE dwsDINTERNALERROR
			'This error should never occur and probably will not affect sound
			'play(?). If it happens please contact DiamondWare.
			PRINT"An internal error has occured"
			PRINT"Please contact DiamondWare"

		CASE dwsDPlayNOSPACEFORSOUND
			'This error is more like a warning, though it may happen on a
			'regular basis, depending on how many sounds you told the STK
			'to allow in dws_Init, how you chose to prioritize sounds and
			'how many sounds are currently being played.
			PRINT"No more room for new digitized sounds right now"

		CASE dwsDSetRateFREQTOLOW
			'The STK will set rate as close as possible to the indicated rate
			'but cannot set a rate that low.
			PRINT"Playback frequency too low"

		CASE dwsDSetRateFREQTOHIGH
			'The STK will set rate as close as possible to the indicated rate
			'but cannot set a rate that high.
			PRINT"Playback frequency too high"

		CASE dwsMPlayNOTADWM
			'You passed the STK a pointer to something which is not a .DWM file!
			PRINT"The file you are attempting to play is not a .DWM"

		CASE dwsMPlayNOTSUPPORTEDVER
			'The STK can't play a .DWM converted using a version of VOC2DWM.EXE
			'newer than itself.  And, although we'll try to maintain backwards
			'compatibility, we may not be able to guarantee that newer versions
			'of the code will be able to play older .DWM files.  In any event,
			'it's a good idea to always convert .MID files with the utility
			'which comes with the library you're linking into your application.
			PRINT"Please reconvert this file using the MID2DWM.EXE which came with this library";

		CASE dwsMPlayINTERNALERROR:
			'This error should never occur and probably will not affect sound
			'play(?). If it happens please contact DiamondWare.
			PRINT"An internal error has occured.  Please contact DiamondWare."

		CASE ELSE
			'This should never occur and probably will not affect sound
			'play(?). If it happens please contact DiamondWare.
			PRINT"I'm confused!  Where am I?  HOW DID I GET HERE????"
			PRINT "The ERROR number is:";errornum

	END SELECT

END SUB



'START OF MAIN

	PRINT
	PRINT "PLAYDWD is Copyright 1994, DiamondWare, Ltd."
	PRINT "All rights reserved."
	PRINT : PRINT : PRINT

	filename$ = LTRIM$(RTRIM$(COMMAND$))
	IF filename$ = "" THEN
		PRINT "Usage PLAYDWD <dwd-file>"
		GOTO ProgramExit
	END IF

	'get the file length
	IF INSTR(filename$, ".DWD") = 0 THEN filename$ = ".DWD"

	OPEN filename$ FOR BINARY AS #1 LEN = 1
	filelen = LOF(1)
	CLOSE #1

	IF filelen = 0 THEN
		PRINT "File Not Found"
		GOTO ProgramExit
	END IF

	IF filelen > 32767 THEN
		PRINT "File Too Big"
		GOTO ProgramExit
	END IF

	OPEN filename$ FOR BINARY AS #1 LEN = 1
	GET #1, 1, buffer(0).buf
	CLOSE #1

	'We need to set every field to -1 in dwsDETECTOVERRIDES struct; this
	'tells the STK to autodetect everything.  Any other value
	'overrides the autodetect routine, and will be accepted on
	'faith, though the STK will verify it if possible.

	dov.baseport = -1
	dov.digdma	 = -1
	dov.digirq	 = -1

	IF DWSDetectHardWare(dov, dres) = 0 THEN
		errnum = dwsErrNo
		DisplayError(errnum)
		GOTO ProgramExit
	END IF

	IF (dres.capability AND dwscapabilityDIG) <> dwscapabilityDIG THEN
		PRINT"DIG support not found"
		GOTO ProgramExit
	END IF

	'The "ideal" struct tells the STK how you'd like it to initialize the
	'sound hardware.  In all cases, if the hardware won't support your	 r
	'request, the STK will go as close as possible.  For example, not all
	'sound boards will support al sampling rates (some only support 5 or
	'6 discrete rates).

	ideal.musictyp	 = 0		 '0=No music, 1=OPL2
	ideal.digtyp		 = 8		 '0=No Dig, 8=8bit
	ideal.digrate 	 = 5000  'sampling rate, in Hz
													 'we could have called dws_DGetRateFromDWD
													 'before initing the STK to get the correct rate
	ideal.dignvoices = 16 	 'number of voices (up to 16)
	ideal.dignchan	 = 1		 '1=mono

	IF dwsInit(dres, ideal) = 0 THEN
		errnum = dwsErrNo
		DisplayError(errnum)
		GOTO ProgramKill
	END IF

	'Set master vol to about 4/5ths of max
	IF dwsXMaster(200) = 0 THEN
		errnum = dwsErrNo
		DisplayError(errnum)
	END IF

	soundseg% = VARSEG(buffer(0).buf)
	soundoff% = VARPTR(buffer(0).buf)
	pointer&	= soundseg% * 256 ^ 2 + soundoff%  'make pointer

	dplay.snd 		 = pointer&
	dplay.count 	 = 1				'0=infinite loop, 1-N num times to play sound
	dplay.priority = 1000
	dplay.presnd	 = 0

	IF dwsDGetRateFromDWD(pointer&, ideal.digrate) = 0 THEN
		errnum = dwsErrNo
		DisplayError(errnum)
		GOTO ProgramKill
	END IF

	IF dwsDSetRate(ideal.digrate)  = 0 THEN
		errnum = dwsErrNo
		DisplayError(errnum)
		GOTO ProgramKill
	END IF

	IF dwsDPlay(dplay)	= 0 THEN
		errnum = dwsErrNo
		DisplayError(errnum)
		GOTO ProgramKill
	END IF

	result% = dwsDSOUNDSTATUSPLAYING
	DO UNTIL (result%  AND dwsDSOUNDSTATUSPLAYING) <> dwsDSOUNDSTATUSPLAYING
		IF dwsDSoundStatus(dplay.soundnum, result%) = 0 THEN
			errnum = dwsErrNo
			DisplayError(errnum)
			GOTO ProgramKill
		END IF
	LOOP

	ProgramKill:

	IF dwsKill = 0 THEN
		errnum = dwsErrNo
		DisplayError(errnum)

		'If an error occurs here, it's either dws_Kill_CANTUNHOOKISR
		'or dws_NOTINITTED.  If it's dws_Kill_CANTUNHOOKISR the user
		'must remove his tsr, and dws_Kill must be called again.  If it's
		'dws_NOTINITTED, there's nothing to worry about at this point.
		IF errnum = dwsKillCANTUNHOOKISR THEN
			GOTO ProgramKill
		END IF
	END IF

	ProgramExit:

END
