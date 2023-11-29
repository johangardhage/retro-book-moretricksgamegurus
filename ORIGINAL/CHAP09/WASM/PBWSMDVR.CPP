
// I N C L U D E S ///////////////////////////////////////////////////////////

#include <stdio.h>
#include <conio.h>

#include "gks.hpp"

#include "iff.hpp"
#include "pixbuf.hpp"

// DEFINITIONS ///////////////////////////////////////////////////////////////

#ifdef __WATCOMC__
#define _getch getch
#endif

// EXTERNS //////////////////////////////////////////////////////////////////

extern "C" void setVideoMode(int);
extern "C" void near writeVideo(char *buffer);
extern "C" void near setVGApalette(char*);
extern "C" void near waitRetrace(void);

#pragma aux writeVideo "*_" parm caller [esi];

// MAIN //////////////////////////////////////////////////////////////////////

void main()
{

   Rect viewPortBounds, worldBounds;
   Rect frame1, frame2, copyBounds;

   Rect rUman1, rUman2, rUman3, rUman4, rUman5, rUman6, rUman7, rUman8;
   Rect rDst1, rDst2, rDst3, rDst4, rDst5, rDst6, rDst7, rDst8;             
   
   rUman1.setRect(0,0,68,106);
   rUman2.setRect(0,1,74,101);
   rUman3.setRect(0,0,68,103);
   rUman4.setRect(0,0,44,104);
   rUman5.setRect(0,0,50,102);
   rUman6.setRect(0,0,84,102);
   rUman7.setRect(0,0,82,100);
   rUman8.setRect(0,0,58,104);

   rDst1.setRect(108,50,176,156);
   rDst2.setRect(108,51,182,151);
   rDst3.setRect(108,50,176,153);
   rDst4.setRect(108,50,152,154);
   rDst5.setRect(108,50,158,152);
   rDst6.setRect(108,50,192,152);
   rDst7.setRect(108,50,190,150);
   rDst8.setRect(108,50,158,154);
   

   pixelBuffer uman1Pix( rUman1 );
   pixelBuffer uman2Pix( rUman2 );
   pixelBuffer uman3Pix( rUman3 );
   pixelBuffer uman4Pix( rUman4 );
   pixelBuffer uman5Pix( rUman5 );
   pixelBuffer uman6Pix( rUman6 );
   pixelBuffer uman7Pix( rUman7 );
   pixelBuffer uman8Pix( rUman8 ); 



   worldBounds.setRect(0,0,640,200);  // worlds
   frame1.setRect(0,0,320,200);
   frame2.setRect(320,0,640,200);
   viewPortBounds.setRect(0,0,320,200);
   
   pixelBuffer worldBuf(worldBounds);
   pixelBuffer viewPortBuf(viewPortBounds);

   lbm uman1BBM;
   lbm uman2BBM;
   lbm uman3BBM;
   lbm uman4BBM;
   lbm uman5BBM;
   lbm uman6BBM;
   lbm uman7BBM;
   
   lbm uman8BBM;
   
   
   lbm lbmTest;
   
   lbmTest.unPacker("scene.lbm", viewPortBuf);
   uman1BBM.unPacker("uman1.bbm", uman1Pix);   // unpack sprite pixels
   uman1BBM.freePalette();
   uman2BBM.unPacker("uman2.bbm", uman2Pix);   // unpack sprite pixels 
   uman2BBM.freePalette();
   uman3BBM.unPacker("uman3.bbm", uman3Pix);   // unpack sprite pixels   
   uman3BBM.freePalette();
   uman4BBM.unPacker("uman4.bbm", uman4Pix);   // unpack sprite pixels   
   uman4BBM.freePalette();
   uman5BBM.unPacker("uman5.bbm", uman5Pix);   // unpack sprite pixels
   uman5BBM.freePalette();
   uman6BBM.unPacker("uman6.bbm", uman6Pix);   // unpack sprite pixels   
   uman6BBM.freePalette();
   uman7BBM.unPacker("uman7.bbm", uman7Pix);   // unpack sprite pixels
   uman7BBM.freePalette();
   
   uman8BBM.unPacker("uman8.bbm", uman8Pix);   // unpack sprite pixels
   uman8BBM.freePalette();
    
   printf("unPacker() done\n");
   
   // load offscreen wolrd
   worldBuf.copyPixels(viewPortBuf, viewPortBounds, frame1);  // cell 1
   worldBuf.copyPixels(viewPortBuf, viewPortBounds, frame2);  // cell 2

   viewPortBuf.copyPixels(worldBuf, frame2, viewPortBounds);
   viewPortBuf.copyTransPixels(uman1Pix, rUman1, rDst1);
   _getch();

   
   setVideoMode( 0x13 );

   lbmTest.convertPal();
   setVGApalette((char*)lbmTest.palette);

   unsigned int cell=1;
   unsigned int ticks=0;  // delay count
   unsigned int i;
   unsigned int l,t,r,b;
   t = 0; b=200;
    
   for (i=0;i<320;i++)
   {
     l=i; r=l+320;    
     copyBounds.setRect(l,t,r,b);
     viewPortBuf.copyPixels(worldBuf,copyBounds,viewPortBounds);
         
         switch(cell)
         {
           case 1:
         viewPortBuf.copyTransPixels(uman1Pix, rUman1, rDst1);
             break;
           case 2:
         viewPortBuf.copyTransPixels(uman2Pix, rUman2, rDst2);
             break;
           case 3:
         viewPortBuf.copyTransPixels(uman3Pix, rUman3, rDst3);
             break;
           case 4:
         viewPortBuf.copyTransPixels(uman4Pix, rUman4, rDst4);
             break;
           case 5:
         viewPortBuf.copyTransPixels(uman5Pix, rUman5, rDst5);
             break;
           case 6:
         viewPortBuf.copyTransPixels(uman6Pix, rUman6, rDst6);
             break;
           case 7:
         viewPortBuf.copyTransPixels(uman7Pix, rUman7, rDst7);
             break;
           case 8:
         viewPortBuf.copyTransPixels(uman8Pix, rUman8, rDst8);
                 break;
         }

         waitRetrace();

         writeVideo((char*)viewPortBuf.pixBuf);
         if (ticks > 4)
         {
         if (cell > 6)
           cell = 1;
         else
           cell++;
         ticks = 0;
         }
         ticks++;
   }
   _getch();
   setVideoMode( 0x03 );
   printf("done\n");
}



