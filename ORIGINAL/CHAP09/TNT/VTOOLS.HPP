#ifndef _VTOOLS_HPP_
#define _VTOOLS_HPP_

// I N C L U D E S ///////////////////////////////////////////////////////////

#include "pixbuf.hpp"

// DEFINITIONS ///////////////////////////////////////////////////////////////

// VGA buffer constants
#define MAX_VGA_BUFFER_SIZE 64000
#define VGA_MODE13_WIDTH 320
#define VGA_MODE13_HEIGHT 200
#define MAX_VGA_SIZE 320*200

// VGA palette DAC regs
#define DAC_MASK_REG  0x3c6
#define DAC_WRITE_REG 0x3c8
#define DAC_DATA_REG  0x3c9 

// wait vertical retrace regs
#define VSYNC_REG     0x3da
#define VSYNC_MASK    0x08

// FUNCTION DEFINITIONS //////////////////////////////////////////////////////

void setVideoMode( int xmode);
void writeVideo( pixelBuffer &p);
void setVGAPalette(char *p);
void waitRetrace();

//////////////////////////////////////////////////////////////////////////////

#endif
