/******************************************************************************
File: 		 dwt.h
Version:	 1.0
Tab stops: every 2 collumns
Project:	 The Sound ToolKit
Copyright: 1994 DiamondWare, Ltd.  All rights reserved.
Written:	 Keith Weiner
Purpose:	 Contains declarations for the DW Timer module
History:	 KW 08/24/94 Started
					 EL 02/17/95 Finalized for v1.0

NB: This code is not compatible with source profilers
******************************************************************************/



#ifndef dwt_INCLUDE

	#define dwt_INCLUDE



	/*
	 . Below are the timer rates supported by DWT.	Anything in between
	 . the listed values will cause the DOS/BIOS clock to tick erratically
	 . and is thus not allowed.  Any value higher than 145.6 Hz means
	 . you have some very special circumstances; DWT won't fit your needs
	 . anyway.
	*/
	#define dwt_18_2HZ		 0						//18.2 Hz
	#define dwt_36_4HZ		 1						//36.4 Hz
	#define dwt_72_8HZ		 2						//72.8 Hz
	#define dwt_145_6HZ 	 3						//145.6 Hz



	#ifdef __cplusplus
		extern "C" {
	#endif


	/*
	 . See #defines, above
	*/
	void _far _pascal dwt_Init(word rate);


	/*
	 . If the program has called dwt_Init, it _MUST_ call dwt_Kill before it
	 . terminates.
	 .
	 . NB: Trap critical errors.	Don't let DOS put up the
	 .		 "Abort, Retry, Fail?" text.	('sides, it'll destroy your pretty gfx)
	*/
	void _far _pascal dwt_Kill(void);


	/*
	 . The following 2 functions affect the timer, but not the music
	*/
	void _far _pascal dwt_Pause(void);

	void _far _pascal dwt_UnPause(void);


	/*
	 .# of ticks since Beginning of World
	*/
	dword _far _pascal dwt_MasterTick(void);



	#ifdef __cplusplus
		}
	#endif



#endif
