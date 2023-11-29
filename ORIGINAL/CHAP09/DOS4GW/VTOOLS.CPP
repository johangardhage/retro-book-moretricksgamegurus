// I N C L U D E S ///////////////////////////////////////////////////////////

#include <i86.h>
#include <dos.h>
#include <string.h>
#include <conio.h>

#include "vtools.hpp"

// FUNCTIONS /////////////////////////////////////////////////////////////////

void setVideoMode( unsigned int xmode )
{ 
   union REGS regs;

  regs.w.ax = xmode; 
  int386( 0x10, &regs, &regs);
}  

//////////////////////////////////////////////////////////////////////////////

void writeVideo( pixelBuffer &p)
{
     char *pVideoMem;
     unsigned int rowIndex, pixelIndex, offset;

     pixelIndex = rowIndex = offset = 0;
     pVideoMem = (char*)0xA0000;
   
     for (rowIndex=0;rowIndex<200;rowIndex++)
     {
         memcpy(pVideoMem + offset, p.pixBuf + offset, 320);
         offset += 320;
     }
}

//////////////////////////////////////////////////////////////////////////////

void setVGAPalette(char *palette)
{
  int i;
  
  outp(DAC_MASK_REG,0xff);
  for (i=0;i<256;i++)
  {
    outp(DAC_WRITE_REG,i);
    outp(DAC_DATA_REG,(*palette++)>>2);
    outp(DAC_DATA_REG,(*palette++)>>2);
    outp(DAC_DATA_REG,(*palette++)>>2);
  }   
}

//////////////////////////////////////////////////////////////////////////////

void waitRetrace()
{
  while(inp(VSYNC_REG) & VSYNC_MASK) {}  // retrace

  while(!(inp(VSYNC_REG) & VSYNC_MASK)) {}  // what start of retrace
}

//////////////////////////////////////////////////////////////////////////////
