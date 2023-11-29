//  
//  This header file defines some machine independant data types to
//  simplify porting from 16-bit to 32-bit compilers.
//
//

#ifndef __COMPAT_H
#define __COMPAT_H

  typedef char  BYTE;
  typedef unsigned char UCHAR;

#ifdef __386__                   // catch 32-bit DOS compilers
  typedef unsigned short WORD;   // 16-bit unsigned integer
  typedef unsigned int   DWORD;  // 32-bit unsigned integer
  typedef short INT;             // 16-bit signed integer
#else
  typedef unsigned int  WORD;    // also 16-bit unsigned integer
  typedef unsigned long DWORD;   // also 32-bit unsigned integer
  typedef int   INT;             // also 16-bit signed integer
#endif

#endif  // __COMPAT_H
