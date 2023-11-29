// Header:   pixbuf.cpp    
// rev.  :   1.00              10.08.94
//
// Compilers: Microsoft Visual C++ 1.5, 2.0, Watcom 10.0, gcc (Sun)
// Environments:  platform independent
// Author:    Michael Norton
// Functionality: 
//     functions for class pixelBuffer
//     function and data members are defined within.
//     class intended as machine independent pixel
//     buffer manipulator.
//
//         pixelDyne  copyright 1993, 1994, 1995
//////////////////////////////////////////////////////////////////////////////

// I N C L U D E S ///////////////////////////////////////////////////////////

#include <stdio.h> 
#include <stdlib.h>   // exit()
#include <assert.h>
#include <string.h>


#include "pixbuf.hpp"

// CLASS MEMBER FUNCTIONS ////////////////////////////////////////////////////

// default constructor
pixelBuffer::pixelBuffer()
{
    printf("default pixelBuffer constructor\n");
    bufSize = 0;
    bufRowBytes = 0;
    width = height = 0;
    pixBuf = NULL;  // reserve a place on that stack!

    // set default bounds
    bounds.right = 0; bounds.top = 0;
    bounds.left = 0; bounds.bottom = 0;
    printf("default contructor pixelBuffer done\n");
}
// pixelBuffer copy constructor
pixelBuffer::pixelBuffer( const pixelBuffer& p )
{
 

    bufSize = p.bufSize;           // get bufSize of object
    bufRowBytes = p.bufRowBytes;   // get bufRowBytes of object
    pixBuf = new unsigned char[bufSize]; // new buffer
    assert( pixBuf != 0 );       // check allocation                                              
    
    bounds = p.bounds;             // new bounds = old
    width = p.bounds.right - p.bounds.left;
    height = p.bounds.bottom - p.bounds.top;                                          
    pixBuf = p.pixBuf;         // new points to old
                                   // change this operation to memcpy
                                   // note in book why this is dangerous
}


// pixelBuffer constructor using buffer size as its calling argument
pixelBuffer::pixelBuffer( unsigned int n )
{
    bufSize = n;
    pixBuf = NULL;
    assert( pixBuf != 0 );
    width = height = 0;

    // set default bounds
    bounds.right = 0; bounds.top = 0;
    bounds.left = 0; bounds.bottom = 0;
}

// pixelBuffer constructor using a Rect as its calling argument    
pixelBuffer::pixelBuffer( const Rect& r )
{
    // determine buffer size
    bufRowBytes = r.right-r.left;  // calculate the # BYTES per row
    width = bufRowBytes;           // logical width = bufRowBytes
    height = r.bottom - r.top;     // calculate logical height 
    bufSize = bufRowBytes * height;// calculate buffer size needed
    
    // set up buffer bounds
    bounds.left = r.left; bounds.top = r.top;
    bounds.right = r.right; bounds.bottom = r.bottom;
    
    // allocate the buffer
    pixBuf = new unsigned char[bufSize];
    assert( pixBuf != 0); 
}



// overloaded assignment operator
pixelBuffer& pixelBuffer::operator=( const pixelBuffer& p )
{
    // allocate a pixelBuffer
    bufSize = p.bufSize;
    bufRowBytes = p.bufRowBytes;
    pixBuf = new unsigned char[bufSize];
    assert( pixBuf != 0 );    // check allocation                                              

    bounds = p.bounds;
    width = p.width;
    height = p.height;
    
    pixBuf = p.pixBuf;

    return *this;
}

void pixelBuffer::copyPixels( const pixelBuffer& srcPixels, 
                          const Rect& srcRect, const Rect& dstRect )
{
   unsigned int srcRowHeight, srcRowBytes, dstRowHeight, dstRowBytes;
   
   srcRowHeight = srcRect.bottom - srcRect.top;    // physical row height of src
   srcRowBytes = srcRect.right - srcRect.left;     // physical row width of src
   dstRowHeight = dstRect.bottom - dstRect.top;    // physical row height of dst
   dstRowBytes = dstRect.right - dstRect.left;     // physical row width of dst

   if (srcRowHeight != dstRowHeight)
   {
     printf("ERROR: copyPixels() h bounds mismatch\n");
     printf("srcRowHeight[%d]\n", srcRowHeight);
     printf("dstRowHeight[%d]\n", dstRowHeight);
     exit(1);
   }
   
   if (srcRowBytes != dstRowBytes)
   {
     printf("ERROR: copyPixels() w bounds mismatch\n");
     printf("srcRowBytes[%d]\n", srcRowBytes);
     printf("dstRowBytes[%d]\n", dstRowBytes);     
     exit(1);
   }
   
   unsigned int rowIndex = 0, rowOffset = 0, srcRowOffset = 0; 
   // set srcRowOffset to index start row of image in src buffer   
   srcRowOffset = srcRect.top * srcPixels.bufRowBytes;
   
   // set rowOffset to index row in dest buffer where image
   // is to be copied                                 
   rowOffset = dstRect.top * bufRowBytes;
   
   // index for row bytes loop
   unsigned int rowBytesIndex = 0;  

   for (rowIndex=0; rowIndex < srcRowHeight; rowIndex++) // row index
   {     
           memcpy(pixBuf + rowOffset + dstRect.left, srcPixels.pixBuf + srcRowOffset +srcRect.left, bufRowBytes); 
       rowOffset += bufRowBytes;      // increment dest row bytes index
       srcRowOffset += srcPixels.bufRowBytes; // increment src row bytes index
   } 
}   


void pixelBuffer::copyTransPixels( const pixelBuffer& srcPixels, 
                          const Rect& srcRect, const Rect& dstRect )
{
   unsigned int srcRowHeight, srcRowBytes, dstRowHeight, dstRowBytes;
   unsigned char pixelByte;
   
   srcRowHeight = srcRect.bottom - srcRect.top;    // physical row height of src
   srcRowBytes = srcRect.right - srcRect.left;     // physical row width of src
   dstRowHeight = dstRect.bottom - dstRect.top;    // physical row height of dst
   dstRowBytes = dstRect.right - dstRect.left;     // physical row width of dst

   if (srcRowHeight != dstRowHeight)
   {
     printf("ERROR: copyTransPixels() h bounds mismatch\n");
     printf("srcRowHeight[%d]\n", srcRowHeight);
     printf("dstRowHeight[%d]\n", dstRowHeight);
     exit(1);
   }
   
   if (srcRowBytes != dstRowBytes)
   {
     printf("ERROR: copyTransPixels() w bounds mismatch\n");
     printf("srcRowBytes[%d]\n", srcRowBytes);
     printf("dstRowBytes[%d]\n", dstRowBytes);     
     exit(1);
   }
  
   unsigned int rowIndex = 0, rowOffset = 0, srcRowOffset = 0; 
   // set srcRowOffset to index start row of image in src buffer   
   srcRowOffset = srcRect.top * srcPixels.bufRowBytes;
   
   // set rowOffset to index row in dest buffer where image
   // is to be copied                                 
   rowOffset = dstRect.top * bufRowBytes;
   
   // index for row bytes loop
   unsigned int rowBytesIndex = 0;  

   for (rowIndex=0; rowIndex < srcRowHeight; rowIndex++) // row index
   {     
       // rowbytes index
       for (rowBytesIndex=0; rowBytesIndex < srcRowBytes; rowBytesIndex++) 
       {
           pixelByte = srcPixels.pixBuf[srcRowOffset + rowBytesIndex + srcRect.left];
           if(pixelByte != 0)
             pixBuf[rowOffset + rowBytesIndex + dstRect.left] = pixelByte;
               
       }
       rowOffset += bufRowBytes;      // increment dest row bytes index
       srcRowOffset += srcPixels.bufRowBytes; // increment src row bytes index
   } 
}   


void pixelBuffer::copyMaskPixels( const pixelBuffer& srcPixels, const pixelBuffer& maskPixels, 
                     const Rect& srcRect, const Rect& maskRect, const Rect& dstRect)
{

   unsigned int srcRowHeight, srcRowBytes;
   unsigned int maskRowHeight, maskRowBytes;   
   unsigned int dstRowBytes;
   unsigned int srcRowOffset, maskRowOffset, dstRowOffset;


   srcRowHeight = srcRect.bottom - srcRect.top;    // physical row height of src
   srcRowBytes = srcRect.right - srcRect.left;     // physical row width of src


   maskRowHeight = maskRect.bottom - maskRect.top;    // physical row height of mask
   maskRowBytes = maskRect.right - maskRect.left;     // physical row width of mask
   
   // align row bytes indexes to their respective start rows
   unsigned int i = 0;
    
   unsigned int rowIndex = 0;                   // index into the logical row of the src image
   unsigned int maskTempOffset = 0;      
   unsigned int rowBytesIndex = 0;      // physical row byte index of the src image
   maskRowOffset = srcRowOffset = 0;  
   
   // align source image for masking
     srcRowOffset = srcRect.top *srcPixels.bufRowBytes;     
     
   // initialize row index to the physical start row of source image in srcBuffer
     maskRowOffset = maskRect.top * maskPixels.bufRowBytes;     
   
   dstRowOffset = 0;
   dstRowBytes = 0; 
   
   // go to the start row of where the buffer is to be written
      dstRowOffset = dstRect.top * bufRowBytes;
   
   char c=0;   
   maskTempOffset = 0;
   for (rowIndex=0; rowIndex < srcRowHeight; rowIndex++) // row index
   {     
    
       for (rowBytesIndex=0; rowBytesIndex<srcRowBytes; rowBytesIndex++) // rowbytes index
       { 
         pixBuf[dstRowOffset+dstRect.left+rowBytesIndex] = 
              ( maskPixels.pixBuf[maskRowOffset + rowBytesIndex + maskRect.left]
             & pixBuf[dstRowOffset+dstRect.left+rowBytesIndex] )
                | srcPixels.pixBuf[srcRowOffset + rowBytesIndex + srcRect.left]; 
       }
       dstRowOffset += bufRowBytes;      // increment src row bytes index
       srcRowOffset += srcPixels.bufRowBytes;
       maskRowOffset += maskPixels.bufRowBytes; 
   } 

}      

//////////////////////////////////////////////////////////////////////////////
