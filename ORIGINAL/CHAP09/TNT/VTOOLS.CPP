// I N C L U D E S ///////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <conio.h>

#include <pharlap.h>  // FarMemCopy()
#include <pldos32.h>

#include "vtools.hpp"
#include "pixbuf.hpp"

// FUNCTIONS /////////////////////////////////////////////////////////////////


void setVideoMode( int xmode)
{
 SWI_REGS regs;
                          
  memset(&regs, 0, sizeof(regs));
  regs.eax = xmode;
  _dx_real_int(0x10, &regs);
}

//////////////////////////////////////////////////////////////////////////////

void writeVideo( pixelBuffer &p)
{
    
  FARPTR pVideoMem, vgaBaseAddr;
  unsigned int rowIndex, pixelIndex, offset;
  unsigned char *pixelBytes;
  

  rowIndex = pixelIndex = offset = 0;
  FP_SET(vgaBaseAddr, 0xA0000, SS_DOSMEM);	 //0x34

  for(rowIndex=0; rowIndex<200;rowIndex++)
  {
	
	FP_SET(pVideoMem, FP_OFF(vgaBaseAddr) + offset, FP_SEL(vgaBaseAddr));
	pixelBytes = p.pixBuf;
	pixelBytes += offset;
	WriteFarMem(pVideoMem, pixelBytes, 320);
    offset += 320;
  }
}

//////////////////////////////////////////////////////////////////////////////

void setVGAPalette(char *palette)
{
  int i;
  
  _outp(DAC_MASK_REG,0xff);
  for (i=0;i<256;i++)
  {
    _outp(DAC_WRITE_REG,i);
    _outp(DAC_DATA_REG,(*palette++)>>2);
    _outp(DAC_DATA_REG,(*palette++)>>2);
    _outp(DAC_DATA_REG,(*palette++)>>2);
  }   
}

//////////////////////////////////////////////////////////////////////////////

void waitRetrace()
{
  while(_inp(VSYNC_REG) & VSYNC_MASK) {}  // retrace

  while(!(_inp(VSYNC_REG) & VSYNC_MASK)) {}  // what start of retrace
}

//////////////////////////////////////////////////////////////////////////////
