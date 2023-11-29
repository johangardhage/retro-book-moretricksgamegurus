//
//  Pull in some header files
//
    #include <stdio.h>
    #include <dos.h>

//
//  Don't need flag in release version of program
//
#ifdef DEBUG
    int KeyboardFlag=1;  // set to true by default
#endif

//
//  Last keypress scancode.  Updated by custom int handler
//
    int volatile ScanCode;

//
//  Pointer to save BIOS interrupt vector
//
    void static interrupt (*OldInt09)(void);

//
//  Acknowledge interrupt to 8259 PIC
//
    #define EOI() outp(0x20,0x20)

//
//  Define constant for the keyboard hardware interrupt
//
    #define KEYBOARD_INT 0x09

//
//  Scancode for Escape keypress
//
    #define ESC_PRESSED 129

//
// This is a stub for a hardware interrupt routine
//
    static void _interrupt NewInt09(void)
    {
      register int x;

      ScanCode=inp(0x60);       // read key code from keyboard
      x=inp(0x61);
      outp(0x61,(x|0x80));
      outp(0x61,x);
      EOI();  // acknowledge interrupt
    }

//
// Install our custom keyboard handler
//
    void InstallKeyboardInt(void)
    {
      // Save the original BIOS interrupt vector
      OldInt09=_dos_getvect(KEYBOARD_INT);

      // Install our own custom interrupt handler
      _dos_setvect(KEYBOARD_INT,NewInt09);
    }

//
// Restore the BIOS keyboard handler
//
    void RestoreKeyboardInt(void)
    {
      _dos_setvect(KEYBOARD_INT,OldInt09);
    }

//
//  Perform required initialization
//
    void Initialize(void)
    {
#ifdef DEBUG
      if(KeyboardFlag)       // flag set from command line
#endif
	// install the custom interrupt handler.
	// note that this _always_ happens in the release version 
	InstallKeyboardInt();
    }

//
//  Clean up before program exit
//
    void CleanUp(void)
    {
#ifdef DEBUG
      if(KeyboardFlag)       // flag set from command line
#endif
	// Restore the BIOS interrupt handler.
	// note that this _always_ happens in the release version 
	RestoreKeyboardInt();
    }

//
//  Main game loop - processes all external events
//
    void EventLoop(void)
    {
      while(ScanCode != ESC_PRESSED)
	printf("Scan code is: %d.  ",ScanCode);
    }

//
//  Main C program entry point
//
    #pragma argsused
    void main(int argc, char *argv[])
    {
// Note that this code is not in release version
#ifdef DEBUG
      int index;

      // loop over all command line parameters
      for(index=1; index < argc; index++)
      { 
        // look at the _second_ character (first could be '-' or '/')
        switch(argv[index][1])
        {
	      case 'k':
	      case 'K':
	        // do not install keyboard handler   
	        KeyboardFlag=0;
	        break;
	      }
      }  // for()
#endif  // DEBUG
      Initialize();
      EventLoop();
      CleanUp();
    }
