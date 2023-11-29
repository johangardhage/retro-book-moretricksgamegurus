// I N C L U D E S ///////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <string.h>
#include <conio.h>

#include "vtools.hpp"
#include "pixbuf.hpp"

// FUNCTIONS /////////////////////////////////////////////////////////////////

void setVideoMode( unsigned int videoMode )
{
   REGS16 regs;
   
  memset(&regs,0,sizeof(REGS16));
  regs.ax = videoMode;
  DosRealIntr(0x10, &regs, 0L, 0);
}

//////////////////////////////////////////////////////////////////////////////

unsigned char *getVideoBuffer()
{
  USHORT pVidMem;   // selector to video memory

  DosMapRealSeg(vidBaseAddr, 64000, &pVidMem);
  return (unsigned char *)MK_FP(pVidMem,0);
}

//////////////////////////////////////////////////////////////////////////////

void writeBlinkerVideo( pixelBuffer &p, unsigned char *pVideoBuf )
{ 
  unsigned int row, offset;
  
  row = offset = 0;  // initialize indices
  
  // loop the 200 rows of video mode 13h
  offset = 0;
  
  for (row=0; row<200; row++)
  { 
    memcpy(pVideoBuf + offset, p.pixBuf + offset, 320);
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
