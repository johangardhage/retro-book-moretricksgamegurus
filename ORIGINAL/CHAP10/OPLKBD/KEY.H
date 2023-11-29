/******************************************************************************
*File:			key.h
*Project: 	Any and all
*Copyright: 1993-1995 DiamondWare, Ltd.  All rights reserved.
*Written: 	Keith Weiner
*Purpose: 	Declares constants for many scancodes, prototypes key API
******************************************************************************/



//ARROWS

#define key_UP_ARROW		0x48
#define key_DOWN_ARROW	0x50
#define key_LEFT_ARROW	0x4b
#define key_RIGHT_ARROW 0x4d


//LETTERS

#define key_A 					0x1e
#define key_B 					0x30
#define key_C 					0x2e
#define key_D 					0x20
#define key_E 					0x12
#define key_F 					0x21
#define key_G 					0x22
#define key_H 					0x23
#define key_I 					0x17
#define key_J 					0x24
#define key_K 					0x25
#define key_L 					0x26
#define key_M 					0x32
#define key_N 					0x31
#define key_O 					0x18
#define key_P 					0x19
#define key_Q 					0x10
#define key_R 					0x13
#define key_S 					0x1f
#define key_T 					0x14
#define key_U 					0x16
#define key_V 					0x2f
#define key_W 					0x11
#define key_X 					0x2d
#define key_Y 					0x15
#define key_Z 					0x2c


//NUMBERS

#define key_1 					0x2
#define key_2 					0x3
#define key_3 					0x4
#define key_4 					0x5
#define key_5 					0x6
#define key_6 					0x7
#define key_7 					0x8
#define key_8 					0x9
#define key_9 					0xa
#define key_0 					0xb


//FUNCTION KEYS

#define key_F1					0x3b
#define key_F2					0x3c
#define key_F3					0x3d
#define key_F4					0x3e
#define key_F5					0x3f
#define key_F6					0x40
#define key_F7					0x41
#define key_F8					0x42
#define key_F9					0x43
#define key_F10 				0x44
#define key_F11 				0x57
#define key_F12 				0x58


//EDITING KEYS

#define key_INSERT			0x52
#define key_HOME				0x47
#define key_PAGEUP			0x49
#define key_DELETE			0x53
#define key_END 				0x4f
#define key_PAGEDOWN		0x51
#define key_FORESLASH 	0x35
#define key_ASTERISK		0x37


//MISCELLANEOUS KEYS

#define key_ESC 				0x01
#define key_SPACE 			0x39
#define key_ENTER 			0x1c


//SHIFT KEYS

#define key_LEFT_SHIFT	0x2a
#define key_RIGHT_SHIFT 0x36
#define key_ALT 				0x38
#define key_CTRL				0x1d


//PAD

#define key_PAD_0 			0x52
#define key_PAD_1 			0x4f
#define key_PAD_2 			0x50
#define key_PAD_3 			0x51
#define key_PAD_4 			0x4b
#define key_PAD_5 			0x4c
#define key_PAD_6 			0x4d
#define key_PAD_7 			0x47
#define key_PAD_8 			0x48
#define key_PAD_9 			0x49

//PUNCTUATION
#define key_SEMI				0x27



void key_Init(void);

void key_Kill(void);


BYTE key_IsDown(BYTE key);

void key_DeRepeat(BYTE key);
