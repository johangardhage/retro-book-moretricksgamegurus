// I N C L U D E S ///////////////////////////////////////////////////////////
#include <stdio.h>
#include <dos.h>
#include <string.h> 
#include <conio.h>

#include "vtools.hpp"
#include "pixbuf.hpp"

// FUNCTIONS /////////////////////////////////////////////////////////////////

void setVideoMode( int xmode)
{   union REGS regs;

  regs.x.ax = xmode; 
  int86( 0x10, &regs, &regs);
}  

//////////////////////////////////////////////////////////////////////////////

void writeVideo( pixelBuffer &p)
{
     unsigned char _far *pVideoMem;
     unsigned int rowIndex,  offset;

     rowIndex = offset = 0;
     
     // use MK_FP macro to form far pointer to video
     pVideoMem = (unsigned char _far*)MK_FP(0xA000,0);
     
     // loop 200 rows of video mode 13h
     for (rowIndex=0;rowIndex<200;rowIndex++)
     {   
         // copy the pixel buffer to the logical row
         _fmemcpy(pVideoMem + offset, p.pixBuf + offset, 320); 
         
         // increment to the next row, 320 pixels per row
         offset += 320;
     }
} 

//////////////////////////////////////////////////////////////////////////////

void setVGAPalette(char *palette)
{
  int i;
  _disable();
  _outp(DAC_MASK_REG,0xff);
  for (i=0;i<256;i++)
  {
    _outp(DAC_WRITE_REG,i);
    _outp(DAC_DATA_REG,(*palette++)>>2);
    _outp(DAC_DATA_REG,(*palette++)>>2);
    _outp(DAC_DATA_REG,(*palette++)>>2);
  }   
  _enable(); 
}

//////////////////////////////////////////////////////////////////////////////

void waitRetrace()
{
  while(_inp(VSYNC_REG) & VSYNC_MASK) {}  // retrace

  while(!(_inp(VSYNC_REG) & VSYNC_MASK)) {}  // what start of retrace
}

//////////////////////////////////////////////////////////////////////////////
