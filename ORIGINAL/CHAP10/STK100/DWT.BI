'******************************************************************************
'File:      dwt.bi
'Version:   1.0
'Tab stops: every 2 collumns
'Project:   The Sound ToolKit
'Copyright: 1994 DiamondWare
'Written:   by Keith Weiner, Erik Lorenzen, and Don Lemons
'Purpose:   Contains declarations for the DW Timer
'History:   DL 09/24/94 Ported to .BI
'           EL 11/27/94 Finalized for v1.0
'
'NOTE: This code is __NOT__ compatible with source profilers
'******************************************************************************



'Below are the timer rates supported by DWT.  Anything in between
'the listed values will cause the DOS/BIOS clock to tick erratically
'and is thus not allowed.  Any value higher than 145.6 Hz means
'you have some very special circumstances; dwt won't fit your needs
'anyway.
      
CONST dwt182HZ	= 0 ' 18.2 Hz
CONST dwt364HZ	= 1 ' 36.4 Hz
CONST dwt728HZ	= 2 ' 72.8 Hz
CONST dwt1456HZ = 3 '145.6 Hz



DECLARE SUB 		 dwtInit				ALIAS "DWT_INIT"			 (BYVAL rate%)

'If the program has called dwt_Init, it _MUST_ call dwt_Kill before it
'terminates.
'
'NB: Trap critical errors.  Don't let DOS put up the
'    "Abort, Retry, Fail?" text.  ('sides, it'll destroy your pretty gfx)
DECLARE SUB 		 dwtKill				ALIAS "DWT_KILL"			 ()


'The following 2 subroutines affect the timer, but not the music
DECLARE SUB 		 dwtPause 			ALIAS "DWT_INIT"			 ()

DECLARE SUB 		 dwtUnPause 		ALIAS "DWT_KILL"			 ()


'# of ticks since Beginning of World
DECLARE FUNCTION dwtMasterTick& ALIAS "DWT_MASTERTICK" ()
