{******************************************************************************
File: 						 findsb.pas
Tab stops:				 every 2 collumns
Project:					 FINDSB utility
Copyright:				 1994 DiamondWare, Ltd.  All rights reserved.
Written:					 Keith Weiner & Erik Lorenzen
Pascal Conversion: David A. Johndrow
Purpose:           Example code to autodetect and print out the sound hardware
History:					 KW 10/21/94 Started
									 DJ 11/11/94 Converted
									 EL 02/02/95 Cleaned up & Finalized
******************************************************************************}



Program FindSb;

uses crt,dws;



Var
	dov:	dws_DOPTR;
	dres: dws_DRPTR;



Procedure DisplayError;
begin
	case dws_ErrNo of
		(*
		 . Note that only those errors which could occur when calling
		 . dws_DetectHardWare are checked for.
		*)
		dws_ALREADYINITTED:
		begin
			(*
			 . If we get here, it means you've called dws_Init() already.  Calling
			 . dws_DetectHardWare() at this point would cause zillions of
			 . problems if we let the call through.
			*)
			writeln('The STK was already initialized\n');
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

		else
		begin
			(*
			 . This should never occur and probably won't affect sound
			 . play(?).  If it happens please contact DiamondWare.
			*)
			writeln('I am confused!  Where am I?  HOW DID I GET HERE????');
			writeln('The ERROR number is:',dws_ErrNo);
		end;

	end;
end;


Function HVal(i: word): char;
begin
	if (i < 10) then
	begin
		HVal := chr(i+48);
	end
	else
	begin
		HVal := chr(i+55);
	end;
end;


Function HexStr(d: word): string;
Var
	s: string;

begin
	s := '    ';

	s[4] := HVal(d mod 16);
	s[3] := HVal((d div 16) mod 16);
	s[2] := HVal((d div 256) mod 16);
	s[1] := HVal((d div 4096) mod 16);

	while (s[1] = '0') and (length(s) > 1) do
	begin
		Delete(s,1,1);
	end;

	HexStr := s;
end;



begin
	writeln;
	writeln('FINDSB is Copyright 1994, DiamondWare, Ltd.');
	writeln('All rights reserved.');
	writeln;
	writeln;

	new(dov);
	new(dres);

	(*
	 . We need to set every field to -1 in dws_DETECTOVERRIDES struct; this
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

	if (dres^.capability and (dws_capability_DIG or dws_capability_FM) <> 0) then
	begin
		writeln('Base port is ',HexStr(dres^.baseport),' hex');
		writeln('');

		if (dres^.mixtyp <> 1) then
		begin
			writeln('The sound hardware supports mixing.');
			writeln('');
		end
		else
		begin
			writeln('The sound hardware does not support mixing.');
			writeln('');
		end;

		if ((dres^.capability and dws_capability_FM) = dws_capability_FM) then
		begin
			writeln('The sound hardware supports FM music playback.');
			writeln;
		end;

		if ((dres^.capability and dws_capability_DIG) = dws_capability_DIG) then
		begin
			writeln('The sound hardware supports digitized sound playback.');
			writeln('The sound hardware uses DMA channel ',dres^.digdma,' and IRQ level ',dres^.digirq,'.');
			writeln;
		end;
	end
	else
	begin
		writeln('No sound hardware detected.');
		writeln;
	end;

end.
