'******************************************************************************
'File:      dws.bi
'Version:   1.0
'Tab stops: every 2 collumns
'Project:   The Sound ToolKit
'Copyright: 1994 DiamondWare, Ltd.  All rights reserved.
'Written:   by Erik Lorenzen, Keith Weiner, and Don Lemons
'Purpose:   Contains declarations for the DW Sound ToolKit
'History:   DL 08/24/94 Started translation
'           KW 08/30/94 Modified
'           DL 11/12/94 Re-Translated
'           EL 11/26/94 Edited for clarity and funtionality
'           EL 11/27/94 Finalized for v1.0
'******************************************************************************



'******************************************************************************
' The following is the complete list of possible values for dwsErrNo.
' dwsErrNo may be set by any dws function.  Check its value whenever
' the return value of a dws function is 0 (error).

CONST dwsEZERO													=  0

' The following 3 errors may be triggered by any dws function
CONST dwsNOTINITTED 										=  1
CONST dwsALREADYINITTED 								=  2
CONST dwsNOTSUPPORTED 									=  3

'The following 4 errors may be triggered by dwsDetectHardWare
CONST dwsDetectHardwareUNSTABLESYSTEM 	=  4
CONST dwsDetectHardwareBADBASEPORT			=  5
CONST dwsDetectHardwareBADDMA 					=  6
CONST dwsDetectHardwareBADIRQ 					=  7

'The following error may be triggered by dwsKill
CONST dwsKillCANTUNHOOKISR							=  8

'The following error may be triggered by any dwsX (mixer) function
CONST dwsXBADINPUT											=  9

'The following 3 errors may be triggered by any dwsD (dig) function
CONST dwsDNOTADWD 											=  10
CONST dwsDNOTSUPPORTEDVER 							=  11
CONST dwsDINTERNALERROR 								=  12

'The following error may be triggered by dwsDPlay
CONST dwsDPlayNOSPACEFORSOUND 					=  13

'The following 2 errors may be triggered by dwsDSetRate
CONST dwsDSetRateFREQTOLOW							=  14
CONST dwsDSetRateFREQTOHIGH 						=  15

'The following 3 errors may be triggered by dwsMPlay
CONST dwsMPlayNOTADWM 									=  16
CONST dwsMPlayNOTSUPPORTEDVER 					=  17
CONST dwsMPlayINTERNALERROR 						=  18
'------------------------------------------------------------------------------


'The follwing section defines bitfields which are used by various
'dws functions.  Each bit in a bitfield, by definition, may be
'set/reset independantly of all other bits.

'The following 2 consts indicate the capabilities of the user's hardware
CONST dwscapabilityFM 									=  1
CONST dwscapabilityDIG									=  2

'The following 2 consts indicate the status of specified digital sounds
CONST dwsDSOUNDSTATUSPLAYING						=  1
CONST dwsDSOUNDSTATUSSEQUENCED					=  2

'The following 2 consts indicate the status of music playback
CONST dwsMSONGSTATUSPLAYING 						=  1
CONST dwsMSONGSTATUSPAUSED							=  2
'******************************************************************************



'The following section TYPEs the structs used by the STK.  In each
'case, the user must create an instance of the struct prior to making
'a call to an STK function which takes a pointer to it.  The STK does
'not keep a pointer to any of these structs internally; after the call
'returns, you may deallocate it, if you wish.


'NB: The STK _does_ keep pointers to songs and digitized sound buffers!
'dwsDetectHardWare can be told _not_ to autodetect particular values
'about the installed hardware.  This is useful if detecting DMA channel,
'for example, consistently causes a machine lockup.  To override the
'autodetect for a setting (causing the STK to accept it on faith),
'set the corresponding field in this struct to the correct value.
'Otherwise, set the field to ffff hex.  Since the autodetect is reliable,
'this is the recommended course of action, except in cases of known
'problems.
TYPE dwsDETECTOVERRIDES
	baseport	 AS INTEGER 		 'base address of sound card (often 220 hex)

	digdma		 AS INTEGER 		 'DMA channel
	digirq		 AS INTEGER 		 'IRQ level

	reserved	 AS STRING * 10  'reserved

END TYPE


'A pointer to this struct is passed to dwsDetectHardWare, which fills
'it in.  It is then passed unmodified to dwsInit.  If you plan on
'writing this struct out to a file, it's important that you write
'the entire contents.  There is information (for internal STK use only)
'in the reserved[] field!
TYPE dwsDETECTRESULTS
	baseport	 AS INTEGER 		 'base address of sound card (often 220 hex)

	capability AS INTEGER 		 'see CONSTs, above

	'The following 3 fields are only valid if FM music is supported
	mustyp		 AS INTEGER 		 '0=none, 1=OPL2
	musnchan	 AS INTEGER 		 '1=mono
	musnvoice  AS INTEGER 		 'number of voices supported by hardware (11 for FM)

	'The following 4 fields are only valid if digitized sound is supported
	dignbits	 AS INTEGER 		 '0=none, 8=8 bit
	dignchan	 AS INTEGER 		 '1=mono
	digdma		 AS INTEGER 		 'DMA channel
	digirq		 AS INTEGER 		 'IRQ level

	mixtyp		 AS INTEGER 		 '1=software, 2+ is hardware

	reserved	 AS STRING * 44  'there are important values in here...

END TYPE


'A pointer to this struct is passed as a parameter to dws_Init.  This
'allows the user to tell the STK to use less than the full capabilities
'of the installed sound hardware, and/or the user's sound board
'may not support every feature of the STK.
TYPE dwsIDEAL
	musictyp	 AS INTEGER 		 '0=No Music, 1=OPL2

	digtyp		 AS INTEGER 		 '0=No Dig, 8=8bit
	digrate 	 AS INTEGER 		 'sampling rate, in Hz
	dignvoices AS INTEGER 		 'number of voices (up to 16)
	dignchan	 AS INTEGER 		 '1=mono

	reserved	 AS STRING * 6

END TYPE


'A pointer to this struct is passed to dwsDPlay.
'Note that the soundnum field is filled in by dws_DPlay as a return value.
TYPE dwsDPLAY
	snd 			 AS LONG				 'pointer to buffer which holds a .DWD file
	count 		 AS INTEGER 		 'number of times to play, or 0=infinite loop
	priority	 AS INTEGER 		 'higher numbers mean higher priority
	presnd		 AS INTEGER 		 'soundnum to sequence sound _after_
	soundnum	 AS INTEGER 		 'dws_DPlay returns a snd number from 10-65535

	reserved	 AS STRING * 20

END TYPE


'A pointer to this struct is passed to dwsMPlay.
TYPE dwsMPLAY
	track 		 AS LONG				 'pointer to buffer which holds a .DWM file
	count 		 AS INTEGER 		 'number of times to play, or 0=infinite loop

	reserved	 AS STRING * 10

END TYPE
'******************************************************************************


'******************************************************************************

'This function is callable at any time.  It returns the number of the
'last error which occured.
DECLARE FUNCTION dwsErrNo%					 ALIAS "DWS_ERRNO"					()
'------------------------------------------------------------------------------


'This subroutine is called at the end of the timer ISR (interrupt service
'routine).  If you're using the optional DWT (DW Timer), this happens
'automagically.  If you wrote your own timer handler routine, you must
'call this function regularly.
DECLARE SUB 		 dwsUpdate					 ALIAS "DWS_UPDATE" 				()
'------------------------------------------------------------------------------


'The following functions have boolean return values.  A return of 0 (false)
'indicates an error.  If this occurs, check dwsErrNo for more info.
DECLARE FUNCTION dwsDetectHardWare%  ALIAS "DWS_DETECTHARDWARE" (SEG dov AS dwsDetectOverrides, SEG dres AS dwsDetectResults)

DECLARE FUNCTION dwsInit% 					 ALIAS "DWS_INIT" 					(SEG dres AS dwsDetectResults, SEG idl AS dwsIdeal)


'If the program has called dws_Init, it _MUST_ call dws_Kill before it
'terminates.
'
'NB: Trap critical errors.  Don't let DOS put up the
'    "Abort, Retry, Fail?" text.  ('sides, it'll destroy your pretty gfx)
DECLARE FUNCTION dwsKill% 					 ALIAS "DWS_KILL" 					()


'The following 3 functions comprise the mixer section of the STK.  A
'value of 0 turns a channel off; a value of 255 is the loudest.
DECLARE FUNCTION dwsXMaster%				 ALIAS "DWS_XMASTER"				(BYVAL volume%)

DECLARE FUNCTION dwsXMusic% 				 ALIAS "DWS_XMUSIC" 				(BYVAL volume%)

DECLARE FUNCTION dwsXDig% 					 ALIAS "DWS_XDIG" 					(BYVAL volume%)


'The following 10 functions comprise the digitized sound functions of
'the STK.  See the documentation for complete details.
DECLARE FUNCTION dwsDPlay%					 ALIAS "DWS_DPLAY"					(SEG dply AS dwsDPlay)

DECLARE FUNCTION dwsDSoundStatus% 	 ALIAS "DWS_DSOUNDSTATUS" 	(BYVAL soundnumber%, SEG result%)

DECLARE FUNCTION dwsDSetRate% 			 ALIAS "DWS_DSETRATE" 			(BYVAL frequency%)

DECLARE FUNCTION dwsDGetRate% 			 ALIAS "DWS_DGETRATE" 			(SEG result%)

'This function is callable at any time.
DECLARE FUNCTION dwsDGetRateFromDWD% ALIAS "DWS_DGETRATEFROMDWD"(BYVAL pointer&, SEG result%)

DECLARE FUNCTION dwsDDiscard% 			 ALIAS "DWS_DDISCARD" 			(BYVAL soundnum%)

DECLARE FUNCTION dwsDDiscardAO% 		 ALIAS "DWS_DDISCARDAO" 		(BYVAL pointer&)

DECLARE FUNCTION dwsDClear% 				 ALIAS "DWS_DCLEAR" 				() 'Affects all sounds

DECLARE FUNCTION dwsDPause% 				 ALIAS "DWS_DPAUSE" 				() 'Affects all sounds

DECLARE FUNCTION dwsDUnPause% 			 ALIAS "DWS_DUNPAUSE" 			() 'Affects all sounds


'The following 5 functions comprise the music functions of the STK.
'See the documentation for complete details.
DECLARE FUNCTION dwsMPlay%					 ALIAS "DWS_MPLAY"					(SEG mply AS dwsMPlay)

DECLARE FUNCTION dwsMSongStatus%		 ALIAS "DWS_MSONGSTATUS"		(SEG result%)

DECLARE FUNCTION dwsMClear% 				 ALIAS "DWS_MCLEAR" 				()

DECLARE FUNCTION dwsMPause% 				 ALIAS "DWS_MPAUSE" 				()

DECLARE FUNCTION dwsMUnPause% 			 ALIAS "DWS_MUNPAUSE" 			()
'******************************************************************************
