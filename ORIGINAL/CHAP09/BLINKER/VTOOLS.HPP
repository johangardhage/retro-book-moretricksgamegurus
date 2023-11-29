#ifndef _VTOOLS_HPP
#define _VTOOLS_HPP

// I N C L U D E S ///////////////////////////////////////////////////////////

#include <blinker.h>
#include <blx286.h>

#include "pixbuf.hpp"

// DEFINITIONS ///////////////////////////////////////////////////////////////

// physical memory attributes of VGA memory
#define vidBaseAddr 0xA000
#define vidBufSize 64000L

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

void setVideoMode( unsigned int vidMode );
void setVGAPalette(char *palette);
unsigned char *getVideoBuffer();
void writeBlinkerVideo( pixelBuffer &p, unsigned char *pVideoBuf );
void waitRetrace();

//////////////////////////////////////////////////////////////////////////////

#endif



