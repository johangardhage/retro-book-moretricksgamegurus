// I N C L U D E S ///////////////////////////////////////////////////////////

#include <stdio.h>
#include <conio.h>
#include <string.h>

#include "gks.hpp"
#include "iff.hpp"
#include "pixbuf.hpp"
#include "vtools.hpp"

// MAIN //////////////////////////////////////////////////////////////////////

void main()
{
   unsigned char *pv;      // video memory pointer

   // Rect objects
   Rect offScreenRect;     // Rect describing the offscreen pixelBuffer object
   Rect rUman7;            // Rect for the sprite
   Rect dstRect;           // Rect for destination of sprite
   
   // Rect objects initializations                  
   offScreenRect.setRect(0,0,320,200);  // initialize bounds of offscreen object
   rUman7.setRect(0,0,82,100);          // initialize bounds of sprite object
   dstRect.setRect(108,50,190,150);
   
   // pixelBuffer obects
   pixelBuffer offScreenPix( offScreenRect );// offscreen pixelBuffer object
   pixelBuffer uman7Pix( rUman7 );           // sprite pixelBuffer object
   
   // lbm objects
   lbm offScreenLBM;       // lbm object for background lbm file
   lbm uman7BBM;           // lbm object for sprite bbm file
   
   // lbm object IFF file reader
   offScreenLBM.unPacker("scene.lbm", offScreenPix);  // unpack offscreen pixels
   uman7BBM.unPacker("uman7.bbm", uman7Pix);   // unpack sprite pixels
   
   // blit sprite object pixels to offscreen object pixels
   offScreenPix.copyTransPixels(uman7Pix, rUman7, dstRect);

   pv = getVideoBuffer();  // initialize pointer to video memory

   setVideoMode( 0x13 );   // set video mode to VGA mode 13h
  
   setVGAPalette(offScreenLBM.palette);  // set 256 color palette from lbm
   
   waitRetrace();
   
   writeBlinkerVideo( offScreenPix, pv );  // write the objects to video
   
   _getch();
   setVideoMode( 0x03 );   // set the video mode to text
  
   printf("done\n");       // done
}

