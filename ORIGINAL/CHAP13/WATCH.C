    //
    // Standard include files
    //
    #include <stdio.h>
    #include <stdlib.h>

    //
    // Simple function for calculating the factorial of a given number
    //
    unsigned long Factorial(unsigned val)
    {
      unsigned long theResult;

      theResult = --val;  // initialize result
      for(; val; val--)   // loop till val = 0 
        theResult*=val;   // accumulate results of multiplies
      return theResult;   // return factorial of 'val'
    }

    //
    //  Main program entry point
    //
    #pragma argsused
    void main(int argc,char *argv[])
    {
      unsigned factor;

      // get the first command line parameter
      factor=(unsigned)atoi(argv[1]);

      // show the results
      printf("result of %u! = %lu\n",factor,Factorial(factor));
    }
