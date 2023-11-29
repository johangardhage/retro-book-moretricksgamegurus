    //
    // Standard include files
    //
    #include <stdio.h>

    //
    // Include file for 32-bit code compatability
    //
    #include "compat32.h"

    //
    // Declare a typical disk file data structure
    //
    typedef struct tagJiffyFile
    {
      char signature[3];
      char dummy1[1];       // pad to double-word boundary
	    WORD length;          // use compiler-independant sizes
    } JiffyFile;

    //
    // Create an instance of data type 'JiffyFile'
    //
    JiffyFile aJiffyFile;

    //
    // Main program entry point
    //
    void main(void)
    {
      FILE *fptr;

      fptr=fopen("foo.dat","rb");
      fread(&aJiffyFile,sizeof(JiffyFile),1,fptr);
      fclose(fptr);
    }
