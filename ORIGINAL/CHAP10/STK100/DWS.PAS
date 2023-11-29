(******************************************************************************
File: 		 dws.pas
Version:	 1.0
Tab stops: every 2 collumns
Project:	 The Sound ToolKit
Copyright: 1994 DiamondWare, Ltd.  All rights reserved.
Written:	 Keith Weiner
Purpose:	 Creates dws.tpu
History:	 KW 10/09/94 Started
					 EL 11/27/94 Finalized for v1.0

NB: The dwt (timer) code is not compatible with source profilers
******************************************************************************)


unit DWS;

interface



const

(*****************************************************************************)
	(*
	 . The following is the complete list of possible values for dws_errno.
	 . dws_errno may be set by any dws_ function.  Check its value whenever
	 . the return value of a dws_ function is 0 (error).
	*)
	dws_EZERO 												 =	0;

	(* The following 3 errors may be triggered by any dws_ function *)
	dws_NOTINITTED										 =	1;
	dws_ALREADYINITTED								 =	2;
	dws_NOTSUPPORTED									 =	3;

	(* The following 4 errors may be triggered by dws_DetectHardWare *)
	dws_DetectHardware_UNSTABLESYSTEM  =	4;
	dws_DetectHardware_BADBASEPORT		 =	5;
	dws_DetectHardware_BADDMA 				 =	6;
	dws_DetectHardware_BADIRQ 				 =	7;

	(* The following error may be triggered by dws_Kill *)
	dws_Kill_CANTUNHOOKISR						 =	8;

	(* The following error may be triggered by any dws_X (mixer) function *)
	dws_X_BADINPUT										 =	9;

	(* The following 3 errors may be triggered by any dws_D (dig) function *)
	dws_D_NOTADWD 										 =	10;
	dws_D_NOTSUPPORTEDVER 						 =	11;
	dws_D_INTERNALERROR 							 =	12;

	(* The following error may be triggered by dws_DPlay *)
	dws_DPlay_NOSPACEFORSOUND 				 =	13;

	(* The following 2 errors may be triggered by dws_DSetRate *)
	dws_DSetRate_FREQTOLOW						 =	14;
	dws_DSetRate_FREQTOHIGH 					 =	15;

	(* The following 3 errors may be triggered by dws_MPlay *)
	dws_MPlay_NOTADWM 								 =	16;
	dws_MPlay_NOTSUPPORTEDVER 				 =	17;
	dws_MPlay_INTERNALERROR 					 =	18;
(*---------------------------------------------------------------------------*)


	(*
	 . The follwing section defines bitfields which are used by various
	 . dws_ functions.	Each bit in a bitfield, by definition, may be
	 . set/reset independantly of all other bits.
	*)

	(* The following 2 consts indicate the capabilities of the user's hardware *)
	dws_capability_FM 								 = $0001;
	dws_capability_DIG								 = $0002;


	(* The following 2 consts indicate the status of specified digital sounds *)
	dws_DSOUNDSTATUSPLAYING 					 = $0001;
	dws_DSOUNDSTATUSSEQUENCED 				 = $0002;


	(* The following 2 consts indicate the status of music playback *)
	dws_MSONGSTATUSPLAYING						 = $0001;
	dws_MSONGSTATUSPAUSED 						 = $0002;

	(*
	 . Below are the timer rates supported by DWT.	Anything in between
	 . the listed values will cause the DOS/BIOS clock to tick erratically
	 . and is thus not allowed.  Any value higher than 145.6 Hz means
	 . you have some very special circumstances; DWT won't fit your needs
	 . anyway.
	*)
	dwt_18_2HZ												 = 0; 					 (*18.2 Hz*)
	dwt_36_4HZ												 = 1; 					 (*36.4 Hz*)
	dwt_72_8HZ												 = 2; 					 (*72.8 Hz*)
	dwt_145_6HZ 											 = 3; 					 (*145.6 Hz*)
(*****************************************************************************)



type

	(*
	 . The following section declares the record types used by the STK.  In each
	 . case, the user must create an instance of the record prior to making
	 . a call to an STK function which takes a pointer to it.  The STK does
	 . not keep a pointer to any of these records internally; after the call
	 . returns, you may deallocate it, if you wish.
	 .
	 . NB: The STK _does_ keep pointers to songs and digitized sound buffers!
	*)


	(*
	 . dws_DetectHardWare can be told _not_ to autodetect particular values
	 . about the installed hardware.	This is useful if detecting DMA channel,
	 . for example, consistently causes a machine lockup.  To override the
	 . autodetect for a setting (causing the STK to accept it on faith),
	 . set the corresponding field in this record to the correct value.
	 . Otherwise, set the field to ffff hex.	Since the autodetect is reliable,
	 . this is the recommended course of action, except in cases of known
	 . problems.
	*)
	dws_DETECTOVERRIDES = record
		baseport : word;									(*base addr of sound card (often 220) hex*)

		digdma	 : word;									(*DMA channel*)
		digirq	 : word;									(*IRQ level*)

		reserved : array[1..10] of word;

	end;


	(*
	 . A pointer to this record is passed to dws_DetectHardWare, which fills
	 . it in.  It is then passed unmodified to dws_Init.	If you plan on
	 . writing this record out to a file, it's important that you write
	 . the entire contents.  There is information (for internal STK use only)
	 . in the reserved[] field!
	*)
	dws_DETECTRESULTS = record
		baseport	 : word;								(*base addr of sound card (often 220) hex*)

		capability : word;								(*see constants, above*)

		(* The following 3 fields are only valid if FM music is supported *)
		mustyp		 : word;								(*0=none, 1=OPL2*)
		musnchan	 : word;								(*1=mono*)
		musnvoice  : word;								(*num hardware voices (11 for FM)*)

		(* The following 4 fields are only valid if digitized sound is supported *)
		dignbits	 : word;								(*0=none, 8=8 bit*)
		dignchan	 : word;								(*1=mono*)
		digdma		 : word;								(*DMA channel*)
		digirq		 : word;								(*IRQ level*)

		mixtyp		 : word;								(*1=software, 2+ is hardware*)

		reserved	 : array[1..44] of byte;(*there are important values in here...*)

	end;


	(*
	 . A pointer to this struct is passed as a parameter to dws_Init.  This
	 . allows the user to tell the STK to use less than the full capabilities
	 . of the installed sound hardware, and/or the user's sound board
	 . may not support every feature of the STK.
	*)
	dws_IDEAL = record
		musictyp	 : word;				 (*0=No Music, 1=OPL2*)

		digtyp		 : word;				 (*0=No Dig, 8=8bit*)
		digrate 	 : word;				 (*sampling rate, in Hz*)
		dignvoices : word;				 (*number of voices (up to 16)*)
		dignchan	 : word;				 (*1=mono*)

		reserved	 : array[1..6] of byte;

	end;


	(*
	 . A pointer to this record is passed to dws_DPlay.
	 . Note that the soundnum field is filled in by dws_DPlay as a return value.
	*)
	dws_DPLAYREC = record
		snd 		 : ^byte; 				 (*pointer to buffer which holds a .DWD file*)
		count 	 : word;					 (*number of times to play, or 0=infinite loop*)
		priority : word;					 (*higher numbers mean higher priority*)
		presnd	 : word;					 (*soundnum to sequence sound _after_*)
		soundnum : word;					 (*dws_DPlay returns a snd number from 10-65535*)

		reserved : array[1..20] of byte;

	end;


	(* A pointer to this record is passed to dws_MPlay. *)
	dws_MPLAYREC = record
		track : ^byte;						 (*pointer to buffer which holds a .DWM file*)
		count : word; 						 (*number of times to play, or 0=infinite loop*)

		reserved : array[1..10] of byte;

	end;
(*****************************************************************************)


	dws_DOPTR = ^dws_DETECTOVERRIDES;
	dws_DRPTR = ^dws_DETECTRESULTS;
	dws_IDPTR = ^dws_IDEAL;
	dws_DPPTR = ^dws_DPLAYREC;
	dws_MPPTR = ^dws_MPLAYREC;

	dws_WDPTR = ^word;
	dws_BTPTR = ^byte;



(*****************************************************************************)


(*
 . This function is callable at any time.  It returns the number of the
 . last error which occured.
*)
function dws_ErrNo : word;
(*---------------------------------------------------------------------------*)


(*
 . This procedure is called at the end of the timer ISR (interrupt service
 . routine).	If you're using the optional DWT (DW Timer), this happens
 . automagically.  If you wrote your own timer handler routine, you must
 . call this function regularly.
*)
procedure dws_Update;
(*---------------------------------------------------------------------------*)


(*
 . Each function in this section has a boolean return value.	A 0 (false)
 . indicates that the function failed in some way.	In this case, call
 . dws_ErrNo to get the specific error.  Otherwise, a return value of 1
 . (true) indicates that all is well.
*)
function dws_DetectHardWare(dov : dws_DOPTR; dr : dws_DRPTR) : word;

function dws_Init(dr : dws_DRPTR; ideal : dws_IDPTR) : word;

(*
 . If the program has called dws_Init, it _MUST_ call dws_Kill before it
 . terminates.
 .
 . NB: Trap critical errors.	Don't let DOS put up the
 .		 "Abort, Retry, Fail?" text.	('sides, it'll destroy your pretty gfx)
*)
function dws_Kill : word;


(*
 . The following 3 functions comprise the mixer section of the STK.  A
 . value of 0 turns a channel off; a value of 255 is the loudest.
*)
function dws_XMaster(volume : word) : word;

function dws_XMusic(volume : word) : word;

function dws_XDig(volume : word) : word;



(*
 . The following 10 functions comprise the digitized sound functions of
 . the STK.  See the documentation for complete details.
*)
function dws_DPlay(dplay : dws_DPPTR) : word;

function dws_DSoundStatus(soundnum : word; result : dws_WDPTR) : word;

function dws_DSetRate(frequency : word) : word;

function dws_DGetRate(result : dws_WDPTR) : word;

(* This function is callable at any time*)
function dws_DGetRateFromDWD(snd : dws_BTPTR; result : dws_WDPTR) : word;

function dws_DDiscard(soundnum : word) : word;

function dws_DDiscardAO(snd : dws_BTPTR) : word;

function dws_DClear : word; 					(*All*)

function dws_DPause : word; 					(*All*)

function dws_DUnPause : word; 				(*All*)


(*
 . The following 5 functions comprise the music functions of the STK.
 . See the documentation for complete details.
*)
function dws_MPlay(mplay : dws_MPPTR) : word;

function dws_MSongStatus(status : dws_WDPTR) : word;

function dws_MClear : word;

function dws_MPause : word;

function dws_MUnPause : word;
(*---------------------------------------------------------------------------*)


(*
 . The following 5 functions/procedures comprise the timer functions of the STK.
 . See the documentation for complete details.
*)

(* See const declarations above for rates*)
procedure dwt_Init(rate : word);

(*
 . If the program has called dwt_Init, it _MUST_ call dwt_Kill before it
 . terminates.
 .
 . NB: Trap critical errors.	Don't let DOS put up the
 .		 "Abort, Retry, Fail?" text.	('sides, it'll destroy your pretty gfx)
*)
procedure dwt_Kill;

(* The following 2 procedures affect the timer, but not the music *)
procedure dwt_UnPause;

procedure dwt_Pause;

(*
 .number of ticks since Beginning of World
*)
function dwt_MasterTick : longint;
(*****************************************************************************)



implementation


function dws_ErrNo : word; external;

procedure dws_Update; external;


function dws_DetectHardWare(dov : dws_DOPTR; dr : dws_DRPTR) : word; external;

function dws_Init(dr : dws_DRPTR; ideal : dws_IDPTR) : word; external;

function dws_Kill : word; external;


function dws_XMaster(volume : word) : word; external;

function dws_XMusic(volume : word) : word; external;

function dws_XDig(volume : word) : word; external;


function dws_DPlay(dplay : dws_DPPTR) : word; external;

function dws_DSoundStatus(soundnum : word; result : dws_WDPTR) : word; external;

function dws_DSetRate(frequency : word) : word; external;

function dws_DGetRate(result : dws_WDPTR) : word; external;

function dws_DGetRateFromDWD(snd : dws_BTPTR; result : dws_WDPTR) : word; external;

function dws_DDiscard(soundnum : word) : word; external;

function dws_DDiscardAO(snd : dws_BTPTR) : word; external;

function dws_DClear : word; external;

function dws_DPause : word; external;

function dws_DUnPause : word; external;


function dws_MSongStatus(status : dws_WDPTR) : word; external;

function dws_MPlay(mplay : dws_MPPTR) : word; external;

function dws_MClear : word; external;

function dws_MPause : word; external;

function dws_MUnPause 	: word; external;


procedure dwt_Init(rate : word); external;

procedure dwt_Kill; external;

procedure dwt_Pause; external;

procedure dwt_UnPause; external;

function dwt_MasterTick : longint; external;


end.
