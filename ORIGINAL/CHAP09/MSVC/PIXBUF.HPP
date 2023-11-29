// Header:   pixbuf.cpp    
// rev.  :   2.00              02.14.95
//
// Compilers: Microsoft Visual C++ 1.51, LARGE model
// Environments: 8086 real mode, 80286 protected mode, far pointer
// Author:    Michael Norton
// Functionality: 
//     functions for class pixelBuffer
//     function and data members are defined within.
//     class intended as machine independent pixel
//     buffer manipulator.
//
//         pixelDyne  copyright 1993, 1994, 1995
//////////////////////////////////////////////////////////////////////////////

#ifndef _PIXBUF_HPP
#define _PIXBUF_HPP

// I N C L U D E S ///////////////////////////////////////////////////////////

#include "gks.hpp"


// CLASS BODY DEFINITION /////////////////////////////////////////////////////

class pixelBuffer{
    friend class Rect;
  private:
    unsigned int bufSize;          // BYTE size of buffer
    unsigned int bufRowBytes;      // # BYTE's of pixels in each row
    unsigned int width, height;    // logical width and height
    Rect bounds;                  // logical rectangular boundaries

  public:
    unsigned char _far *pixBuf;          // buffer for pixels
    // default constructor
    pixelBuffer();
    // constructor based on buffer size
    pixelBuffer( unsigned int n );
    // constructor based on rect
    pixelBuffer( const Rect& r);
    // copy constructor
    pixelBuffer( const pixelBuffer& );
    
    // over loaded assignment operator
    pixelBuffer& operator=(const pixelBuffer&);

    // copy pixel BYTEs
    void copyPixels( const pixelBuffer&,const Rect&, const Rect& );
    void copyTransPixels( const pixelBuffer&,const Rect&, const Rect& );
    void copyMaskPixels( const pixelBuffer&, const pixelBuffer&, 
                            const Rect&, const Rect&, const Rect& );
    

    // dump buffer contents to the screen
    void writeVideo();
    void pharWriteVideo();

    // default destructor
    ~pixelBuffer(){};
};
#endif

//////////////////////////////////////////////////////////////////////////////


