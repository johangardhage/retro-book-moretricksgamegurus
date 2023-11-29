#ifndef _CLASSLBM_HPP_
#define _CLASSLBM_HPP_
// CREDITS ///////////////////////////////////////////////////////////////////
//
//  Technical information on reading ilbm files is difficult to locate,
//  code here in was peiced together from highly valued assistance from
//  Jaimi McEntie, whose readIFF() function from ACK3D was the model for this
//  code.  Without his assistance I would have been bald from pulling my 
//  hair out.
//  Also, technical references in this code are from an Electronic Arts
//  document, " 'ILBM' IFF Interleaved Bitmap'", January 17, 1986.
//  EA denies existence of this document. I aquired it by calling EA
//  tech support during an eclipse which happens in the northern hemisphere
//  only once a century.
//  
//////////////////////////////////////////////////////////////////////////////

// I N C L U D E S ///////////////////////////////////////////////////////////
                       
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

#include "pixbuf.hpp"

// DEFINITIONS ///////////////////////////////////////////////////////////////

const unsigned int BYTE_LEN = 4;
const unsigned char BMHD[] = "BMHD";
const unsigned char CMAP[] = "CMAP";
const unsigned char BODY[] = "BODY";
const unsigned int MAX_TRIPLET_SIZE = 768;

// STRUCTURES ////////////////////////////////////////////////////////////////
                       
typedef struct 
{
  unsigned long type;
  long  cksize;
  unsigned long    subtype; 
} form_chunk;

typedef struct 
{
  char ckID[4];
  unsigned long  ckSize;
} ChunkHeader;


// BitMapHeader //////////////////////////////////////////////////////////////

typedef struct 
{
    unsigned short w, h;                   // raster width & height in pixels 
    unsigned short  x, y;                  // position for this image 
    unsigned char nPlanes;                 // # source bitplanes 
    unsigned char masking;                 // masking technique 
    unsigned char compression;             // compression algoithm 
    unsigned char pad1;                    // UNUSED.  For consistency, put 0 here.
    unsigned short transparentColor;       // transparent "color number" 
    unsigned char xAspect, yAspect;        // aspect ratio, a rational number x/y
    unsigned short pageWidth, pageHeight;  // source "page" size in pixels 
} BitMapHeader;

// CLASS BODY DEFINITION /////////////////////////////////////////////////////

class lbm {
    friend class pixelBuffer;
  // data members
  public:
  unsigned int width;
  unsigned int height; 
  unsigned int bufSize;
   
  unsigned char *lbmBuf;
  char *palette;
  
  // constructors
  lbm();
  
  // member functions
  void unPacker( char *filename);
  void unPacker( char *filename, pixelBuffer&);
  long intelByteOrderLong( long hostBytesLong );
  int  intelByteOrder( int hostBytes );
  void convertPal();
  void freePalette();
  void freeBuf();
  
  // destructors
  ~lbm();
};

#endif

