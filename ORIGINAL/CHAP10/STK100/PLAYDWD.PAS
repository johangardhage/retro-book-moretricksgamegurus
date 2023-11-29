(******************************************************************************
File: 						 playdwd.pas
Tab stops:				 every 2 collumns
Project:					 DWD Player
Copyright:				 1994 DiamondWare, Ltd.  All rights reserved.*
Written:					 Keith Weiner & Erik Lorenzen
Pascal Conversion: David A. Johndrow
Purpose:					 Contains simple example code to show how to load/play a
									 .DWD file
History:					 KW 10/21/94 Started playdwd.c
									 DJ 11/12/94 Translated to Pascal
									 EL 01/12/95 Finalized

Notes
-----

The bulk of this file is error checking logic.

However, this code isn't really robust when it comes to standard error checking
and particularly recovery, software engineering technique, etc. The STK will
handle songs larger than 64K (but not digitized sounds).	Also, exitting
and cleanup is not handled robustly in this code.  The code below can
only be validated by extremely careful scrutiny to make sure each case is
handled properly.

But all such code would make this example file less clear; its purpose was
to illustrate how to call the STK, not how to write QA-proof software.


*Permission is expressely granted to use DisplayError or any derivitive made
 from it to registered users of the STK.
******************************************************************************)



Program PlayDWD;

uses crt,dws;



var
	ExitSave: pointer;

	ch: 				char;
	fp: 				file;
	dov:				dws_DOPTR;
	dres: 			dws_DRPTR;
	ideal:			dws_IDPTR;
	dplay:			dws_DPPTR;
	errno:			word;
	input:			integer;
	sound:			pointer;
	result: 		word;
	soundsize:	longint;
	DWDInitted: boolean;
	KeepGoing:	boolean;



Function Exist(FileName: string): boolean;
Var
	Fil: File;
begin
	Assign(Fil,FileName);
	{*$I- }
	Reset(Fil);
	Close(Fil);
	{$I+ }

	Exist := (IOResult = 0);
end;


Procedure DisplayError;
begin
	case dws_ErrNo of

		dws_EZERO:
		begin
			(*
			 . This should not have happened, considering how we got here!
			*)
			writeln('I am confused!  Where am I?  HOW DID I GET HERE????');
			writeln('The ERROR number is:',dws_ErrNo);
		end;

		dws_NOTINITTED:
		begin
			(*
			 . If we get here, it means you haven't called dws_Init().
			 . The STK needs to initialize itself and the hardware before
			 . it can do anything.
			*)
			writeln('The STK was not initialized');
		end;

		dws_ALREADYINITTED:
		begin
			(*
			 . If we get here, it means you've called dws_Init() already.  Calling
			 . dws_DetectHardWare() at this point would cause zillions of
			 . problems if we let the call through.
			*)
			writeln('The STK was already initialized');
		end;

		dws_NOTSUPPORTED:
		begin
			(*
			 . If we get here, it means that either the user's machine does not
			 . support the function you just called, or the STK was told not to
			 . support it in dws_Init.
			*)
			writeln('Function not supported');
		end;

		dws_DetectHardware_UNSTABLESYSTEM:
		begin
			(*
			 . Please report it to DiamondWare if you get here!
			 .
			 . Ideally, you would disable control-C here, so that the user can't
			 . hit control-alt-delete, causing SmartDrive to flush its (possibly
			 . currupt) buffers.
			*)
			writeln('The system is unstable!');
			writeln('Please power down now!');

			while (1 = 1) do
			begin
			end;
		end;

		(*
		 . The following three errors are USER/PROGRAMMER errors.  You forgot
		 . to fill the cardtyp struct full of -1's (except in those fields
		 . you intended to override, or the user (upon the unlikly event that
		 . the STK was unable to find a card) gave you a bad overide value.
		*)
		dws_DetectHardware_BADBASEPORT:
		begin
			(*
			 . You set dov.baseport to a bad value, or
			 . didn't fill it with a -1.
			*)
			writeln('Bad port address');
		end;

		dws_DetectHardware_BADDMA:
		begin
			(*
			 . You set dov.digdma to a bad value, or
			 . didn't fill it with a -1.
			*)
			writeln('Bad DMA channel');
		end;

		dws_DetectHardware_BADIRQ:
		begin
			(*
			 . You set dov.digirq to a bad value, or
			 . didn't fill it with a -1.
			*)
			writeln('Bad IRQ level');
		end;

		dws_Kill_CANTUNHOOKISR:
		begin
			(*
			 . The STK points the interrupt vector for the sound card's IRQ
			 . to its own code in dws_Init.
			 .
			 . dws_Kill was unable to restore the vector to its original
			 . value because other code has hooked it after the STK
			 . initialized(!)  This is really bad.	Make the user get rid
			 . of it and call dws_Kill again.
			*)
			writeln('Get rid of your TSR, pal!');
			writeln('(Press any key)');
			repeat
			until (keypressed);
		end;

		dws_X_BADINPUT:
		begin
			(*
			 . The mixer funtion's can only accept volumes between 0 & 255,
			 . the volume will remain unchanged.
			*)
			writeln('Bad mixer level');
		end;

		dws_D_NOTADWD:
		begin
			(* You passed the STK a pointer to something which is not a .DWD file! *)
			writeln('The file you are attempting to play is not a .DWD');
		end;

		dws_D_NOTSUPPORTEDVER:
		begin
			(*
			 . The STK can't play a .DWD converted using a version of VOC2DWD.EXE
			 . newer than itself.  And, although we'll try to maintain backwards
			 . compatibility, we may not be able to guarantee that newer versions
			 . of the code will be able to play older .DWD files.  In any event,
			 . it's a good idea to always convert .VOC files with the utility
			 . which comes with the library you're linking into your application.
			*)
			writeln('Please reconvert this file using the VOC2DWD.EXE which came with this library');
		end;

		dws_D_INTERNALERROR:
		begin
			(*
			 . This error should never occur and probably will not affect sound
			 . play(?).  If it happens please contact DiamondWare.
			*)
			writeln('An internal error has occured');
			writeln('Please contact DiamondWare');
		end;

		dws_DPlay_NOSPACEFORSOUND:
		begin
			(*
			 . This error is more like a warning, though it may happen on a
			 . regular basis, depending on how many sounds you told the STK
			 . to allow in dws_Init, how you chose to prioritize sounds and
			 . how many sounds are currently being played.
			*)
			writeln('No more room for new digitized sounds right now');
		end;

		dws_DSetRate_FREQTOLOW:
		begin
			(*
			 . The STK will set rate as close as possible to the indicated rate
			 . but cannot set a rate that low.
			*)
			writeln('Playback frequency too low');
		end;

		dws_DSetRate_FREQTOHIGH:
		begin
			(*
			 . The STK will set rate as close as possible to the indicated rate
			 . but cannot set a rate that high.
			*)
			writeln('Playback frequency too high');
		end;

		dws_MPlay_NOTADWM:
		begin
			(* You passed the STK a pointer to something which is not a .DWM file! *)
			writeln('The file you are attempting to play is not a .DWM');
		end;

		dws_MPlay_NOTSUPPORTEDVER:
		begin
			(*
			 . The STK can't play a .DWM converted using a version of VOC2DWM.EXE
			 . newer than itself.  And, although we'll try to maintain backwards
			 . compatibility, we may not be able to guarantee that newer versions
			 . of the code will be able to play older .DWM files.  In any event,
			 . it's a good idea to always convert .MID files with the utility
			 . which comes with the library you're linking into your application.
			*)
			writeln('Please reconvert this file using the MID2DWM.EXE which came with this library');
		end;

		dws_MPlay_INTERNALERROR:
		begin
			(*
			 . This error should never occur and probably will not affect sound
			 . play(?). If it happens please contact DiamondWare.
			*)
			writeln('An internal error has occured.');
			writeln('Please contact DiamondWare');
		end;

		else
		begin
			(*
			 . This should never occur and probably will not affect sound
			 . play(?). If it happens please contact DiamondWare.
			*)
			writeln('I am confused!  Where am I?  HOW DID I GET HERE????');
			writeln('The ERROR number is:',dws_ErrNo);
		end;
	end;
end;


procedure ExitPlay; far;

label TryToKillAgain;

begin
	ExitProc := ExitSave;

	TryToKillAgain:

	if (dws_Kill <> 1) then
	begin
		(*
		 . If an error occurs here, it's either dws_Kill_CANTUNHOOKISR
		 . or dws_NOTINITTED.  If it's dws_Kill_CANTUNHOOKISR the user
		 . must remove his tsr, and dws_Kill must be called again.	If it's
		 . dws_NOTINITTED, there's nothing to worry about at this point.
		*)
		DisplayError;

		if (dws_ErrNo = dws_Kill_CANTUNHOOKISR) then
		begin
			goto TryToKillAgain;
		end;
	end;

	if (sound <> nil) then
	begin
		freemem(sound, soundsize);
	end;

	dispose(dplay);
	dispose(ideal);
	dispose(dres);
	dispose(dov);
end;



begin
	ExitSave := ExitProc;
	ExitProc := @ExitPlay;

	writeln;
	writeln('PLAYDWD is Copyright 1994, DiamondWare, Ltd.');
	writeln('All rights reserved.');
	writeln;
	writeln;

	new(dov);
	new(dres);
	new(ideal);
	new(dplay);

	sound := nil;

	if (ParamCount = 0) then
	begin
		writeln('Usage PLAYDWD <dwd-file>');
		halt(65535);
	end;

	if Exist(ParamStr(1)) then
	begin
		Assign(fp, ParamStr(1));
		Reset(fp,1);
		soundsize := filesize(fp);

		(* Please note we don't check to see if we get the memory we need. *)
		Getmem(sound, soundsize);
		BlockRead(fp,sound^,soundsize);
		Close(fp);
	end
	else
	begin
		writeln('Unable to open '+ParamStr(1));
		halt(65535);
	end;

	(*
	 . We need to set every field to -1 in dws_DETECTOVERRIDES record; this
	 . tells the STK to autodetect everything.	Any other value
	 . overrides the autodetect routine, and will be accepted on
	 . faith, though the STK will verify it if possible.
	*)
	dov^.baseport := 65535;
	dov^.digdma 	:= 65535;
	dov^.digirq 	:= 65535;

	if (dws_DetectHardWare(dov, dres) = 0) then
	begin
		DisplayError;
		halt(65535);
	end;

	(*
	 . The "ideal" record tells the STK how you'd like it to initialize the
	 . sound hardware.	In all cases, if the hardware won't support your
	 . request, the STK will go as close as possible.  For example, not all
	 . sound boards will support al sampling rates (some only support 5 or
	 . 6 discrete rates).
	*)
	ideal^.musictyp 	:= 0; 		(*0=No music, 1=OPL2*)
	ideal^.digtyp 		:= 8; 		(*0=No Dig, 8=8bit*)
	ideal^.digrate		:= 11000; (*sampling rate, in Hz*)
															(*we could have called dws_DGetRateFromDWD*)
															(*before dws_Init to get the correct rate*)
	ideal^.dignvoices := 1; 		(*number of voices (up to 16)*)
	ideal^.dignchan 	:= 1; 		(*1=mono*)

	if (dws_Init(dres, ideal) = 0) then
	begin
		DisplayError;
		halt(65535);
	end;

	(*
	 . Set master volume to about 4/5ths max
	*)
	if (dws_XMaster(200) = 0) then
	begin
		DisplayError;
	end;

	dplay^.snd			:= sound;
	dplay^.count		:= 1; 			(* 0=infinite loop, 1-N num times to play sound *)
	dplay^.priority := 1000;
	dplay^.presnd 	:= 0;

	if (dws_DGetRateFromDWD(sound, @ideal^.digrate) = 0) then
	begin
		DisplayError;
		halt(65535);
	end;

	if (dws_DSetRate(ideal^.digrate) = 0) then
	begin
		DisplayError;
		halt(65535);
	end;

	if (dws_DPlay(dplay) = 0) then
	begin
		DisplayError;
		halt(65535);
	end;

	repeat
	begin
		if(dws_DSoundStatus(dplay^.soundnum, @result) = 0) then
		begin
			DisplayError;
			halt(65535);
		end;
	end;
	until (result = 0) or (keypressed);

	halt(65535);
end.
