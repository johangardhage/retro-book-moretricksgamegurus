// I N C L U D E S ///////////////////////////////////////////////////////////
#include <stdio.h>
#include <conio.h>
#include <dos.h>

#include "gks.hpp"
#include "vtools.hpp"
#include "pixbuf.hpp"
#include "iff.hpp"

#ifdef __WATCOMC__
#define _getch getch
#endif

// MAIN //////////////////////////////////////////////////////////////////////

void main()
{           
   // Rect objects
   Rect offScreenRect;     // Rect describing the offscreen pixelBuffer object
   Rect spriteRect;       // Rect describing the sprite pixelBuffer object
   Rect dstRect;
   		  
   // Rect objects initializations                  
   offScreenRect.setRect(0,0,320,200);  // initialize bounds of offscreen object
   spriteRect.setRect(0,0,82,100);          // initialize bounds of sprite object
   dstRect.setRect(108,50,190,150);

   // pixelBuffer obects
   pixelBuffer offScreenPix( offScreenRect );// offscreen pixelBuffer object
   pixelBuffer spritePix( spriteRect );  // sprite pixelBuffer object
   
   // lbm objects
   lbm offScreenLBM;       // lbm object for offscreen pixel file
   lbm spriteBBM;    // lbm object for sprite pixel file
   
   // lbm object IFF file reader
   offScreenLBM.unPacker("scene.lbm", offScreenPix);  // unpack offscreen pixels
   spriteBBM.unPacker("uman7.bbm", spritePix);   // unpack sprite pixels
   
   // blit sprite object pixels to offscreen object pixels
   offScreenPix.copyTransPixels(spritePix, spriteRect, dstRect);
   
   // set the video to mode 13h VGA 320 x 200
   setVideoMode( 0x13 );
   
   setVGAPalette((char*)offScreenLBM.palette);
   // write the pixel buffer to the screen
   writeVideo(offScreenPix); 
   
   // pause
   _getch();
      
   // return to standard text video mode 03h
   setVideoMode( 0x03 ); 

   printf("done\n");
}  



