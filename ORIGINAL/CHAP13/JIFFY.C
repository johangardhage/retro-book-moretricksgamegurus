    //
    // Standard include files
    //
    #include <stdio.h>

    //
    // Declare a typical disk file data structure
    //
    typedef struct tagJiffyFile
    {
      char signature[3];
	    unsigned int length;
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
      FILE *f;

      f=fopen("foo.dat","rb");
      fread(&aJiffyFile,sizeof(JiffyFile),1,f);
      fclose(f);
    }
