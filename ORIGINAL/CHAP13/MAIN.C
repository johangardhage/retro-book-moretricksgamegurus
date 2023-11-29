//
// Standard include files
//
#include <stdio.h>

//
// Ansi C function prototypes
//
void foo1(void);
void foo2(void);

//
// Main program entry point
//
void main(void)
{
  foo1();
  foo2();
#ifdef DEBUG
  printf("foo you three times\n");
#endif
  printf("done\n");
}

