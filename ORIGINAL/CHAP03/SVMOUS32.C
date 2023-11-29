/*  SVMOUS32.C - 32-bit example implementing a mouse pointer at 640x480x256
                 resolution.  Includes example of direct support for non-VESA
                 cards.   */

#include <stdio.h>
#include <stdlib.h>
#include <i86.h>
#include <malloc.h>

#define TRIDENT 1
#define VESA    2
#define HUALON  3

struct {                      //  Mouse info block
   short int mx, my;          //  Current x/y position
   short int px, py;          //  Previous x/y position
   char lmb, rmb;             //  Button press status, left and right
   char lmr, rmr;             //  Button release status, left & right
   short int h, w;            //  Height and width of pointer image
   char *image;               //  Pointer to image data
   char *background;          //  Pointer to saved background
   } m;

long stx[480];
unsigned short sts[480];
char stb[480];
char stp[480];

char lastp = 0;

char SVGAcard;

#define B 14                      //  Mouse border color
#define I 1                       //  Mouse "inside" color

/*  Define an image for our mouse pointer... */

char image[160] = { B,B,0,0,0,0,0,0,0,0,
                    B,I,B,0,0,0,0,0,0,0,
                    B,I,I,B,0,0,0,0,0,0,
                    B,I,I,I,B,0,0,0,0,0,
                    B,I,I,I,I,B,0,0,0,0,
                    B,I,I,I,I,I,B,0,0,0,
                    B,I,I,I,I,I,I,B,0,0,
                    B,I,I,I,I,I,I,I,B,0,
                    B,I,I,I,I,I,I,B,0,B,
                    B,I,I,I,I,I,B,0,0,0,
                    B,I,B,B,I,I,B,0,0,0,
                    B,B,0,0,B,I,I,B,0,0,
                    B,0,0,0,0,B,I,B,0,0,
                    0,0,0,0,0,B,I,I,B,0,
                    0,0,0,0,0,0,B,I,B,0,
                    0,0,0,0,0,0,B,B,0,0   };



void SVGA_InitTables(void);
void SVGA_SetMode(void);
void SVGA_DisplayBlock(char *, int, int);
void SVGA_DisplayLine(char *, unsigned int);
void SVGA_DisplayScreen(char *);
void DisplayMouse(char *, int, int);
void UnDisplayMouse(char *, int, int);
void MouseImageInit(void);
void LoopCheck(char *);
short int VESA_SetMode(short int);
void VESA_SetPage(char);


main(int argc, char *argv[])
{
FILE *fp;
unsigned int j;
char  *Buffer;

SVGAcard = VESA;          // A sensible default!

if (argc == 2)
   {
   if (strcmp(argv[1], "TRIDENT") == NULL)
      SVGAcard = TRIDENT;
   if (strcmp(argv[1], "HUALON") == NULL)
      SVGAcard = HUALON;
   }

if ( (Buffer = malloc(307200)) == NULL)
   exit(0);

for (j=0;j<480;j++)
   memset(Buffer+(j*640), (unsigned char)j, 640);

MouseImageInit();
MouseInit();

SVGA_InitTables();

SVGA_SetMode();       // For this example, 640x480 only

for (j=0; j<480; j++)
   SVGA_DisplayLine(Buffer, j);

m.mx = 0;  m.my = 0;      // Start at the top-left of the screen...
m.px = 0;  m.py = 0;

DisplayMouse(Buffer, m.mx, m.my);

while(!m.lmr)
   {
   LoopCheck(Buffer);
   }

RestoreTextMode();
}


void SVGA_InitTables(void)
{

unsigned int j;
char page=0;
long runner=0L;

for (j=0; j<480; j++)
    {
    stx[j] = j * 640;
    sts[j] = (short int)runner;
    if (runner+640 > 65535)
       {
       stb[j] = 1;
       runner -= 65536;
       }
    else
       stb[j] = 0;

    if (j != 0 && stb[j-1] == 1)
       page++;

    stp[j] = page;
    runner += 640;
    }
}

/***  MODE SELECTION AND PAGE-SWITCHING FUNCTIONS  ***/

void SVGA_SetMode(void)
{

union REGS r;

switch(SVGAcard)
   {
   case TRIDENT:
      r.w.ax = 0x005d;
      int386(0x10, &r, &r);
      break;

   case VESA:
      VESA_SetMode(0x0101);
      break;

   case HUALON:
      r.w.ax = 0x002d;
      int386(0x10, &r, &r);
      break;

   }
}


void SVGA_SetPage(char page)
{
int i, n;

lastp = page;

n = page;

switch(SVGAcard)
   {
   case TRIDENT:
      _disable();
      outp(0x03ce, 0x06);
      i = inp(0x03cf);
      outpw(0x03ce, (( i | 0x04) << 8) | 0x0006);
      outp(0x03c4, 0x0b);
      i=inp(0x03c5);
      outpw(0x03c4, ((n ^ 0x02) << 8) | 0x000e);
      _enable();
      break;

   case VESA:
      VESA_SetPage(n);
      break;

   case HUALON:
      n = (n << 12) | 0x00ee;
      outpw(0x03c4, n);
      break;

   }

}


short int VESA_SetMode(short int Mode)
{

union REGS r;

r.h.ah = 0x4F;
r.h.al = 2;
r.w.bx = Mode;

int386(0x10, &r, &r);

if (r.h.ah == 0)
   return(0);
else
   return(1);
}


void VESA_SetPage(char Page)
{

union REGS r;

r.h.ah = 0x4F;
r.h.al = 5;
r.w.bx = 0;
r.w.dx = Page;

int386(0x10, &r, &r);

}


/***  SCREEN UPDATING FUNCTIONS  ***/

void SVGA_DisplayBlock(char *Buffer, int start, int end)
{
char *ScrPtr;
unsigned short int Lngth, wrki;
long lgth;

if (start < 0) start = 0;    // Just to protect us from ourselves!
if (end > 479) end = 479;

if (lastp != stp[start])
   SVGA_SetPage(stp[start]);

/*  1.  The entire block is in one page - great!  */
if (stp[start] == stp[end])
   {
   ScrPtr = 0xA0000;
   Lngth = (stx[end] - stx[start]) + 640;
   memcpy(ScrPtr+sts[start], Buffer+stx[start], Lngth);
   }

else

/*  2.  The block straddles a page boundary - not so great!  */
   {
   if (stp[end] - stp[start] == 1)
      {
      /*  OK, do the start page first... */
      ScrPtr = 0xA0000 + sts[start];
      Lngth = (65536 * (stp[start] + 1)) - stx[start];
      memcpy(ScrPtr, Buffer+stx[start], Lngth);

     /*  Right - swap pages and do the next one...  */

      SVGA_SetPage(stp[start]+1);

      ScrPtr = 0xA0000;
      lgth = (stx[end] - stx[start] - Lngth) + 640;
      memcpy(ScrPtr, Buffer+stx[start]+Lngth, lgth);
      }
   else

/*  3.  This is a large block, and multiple page switches are required.
        Update it line-by-line, or the whole screen */
      {
      if (end - start > 150)
         SVGA_DisplayScreen(Buffer);
      else
         for (wrki=start; wrki<= end; wrki++)
             SVGA_DisplayLine(Buffer, wrki);
      }
   }
}


void SVGA_DisplayLine(char *Buffer, unsigned int line)
{

char page, *ScrPtr;
long lgth;

page = stp[line];
if (page != lastp)
   SVGA_SetPage(page);

ScrPtr = 0xA0000 + sts[line];

if (stb[line] == 0)
   memcpy(ScrPtr, Buffer+stx[line], 640);

else
   {
   lgth = (65536 * (page+1)) - stx[line];
   memcpy(ScrPtr, Buffer+stx[line], lgth);

   SVGA_SetPage(++page);
   ScrPtr = 0xA0000;
   memcpy(ScrPtr, Buffer+stx[line]+lgth, 640-lgth);
   }
}


void SVGA_DisplayScreen(char *Buffer)
{

/*  This function is hard-wired to update an entire 640x480 screen  */

SVGA_SetPage(0);
memcpy(0xA0000, Buffer, 65536);
SVGA_SetPage(1);
memcpy(0xA0000, Buffer+65536, 65536);
SVGA_SetPage(2);
memcpy(0xA0000, Buffer+131072, 65536);
SVGA_SetPage(3);
memcpy(0xA0000, Buffer+196608, 65536);
SVGA_SetPage(4);
memcpy(0xA0000, Buffer+262144, 45056);
}

/***  MOUSE CURSOR DISPLAYING FUNCTIONS  ***/

void DisplayMouse(char *Buffer, int x, int y)
{
int j, k, c=0, bb, rb;
char *wrk;

rb = x+m.w;
if (rb > 639)
   rb = 639;

bb = y+m.h;
if (bb > 479)
   bb = 479;

for (j=y; j<bb; j++)
    {
    wrk = Buffer + stx[j];
    c = (j-y) * m.w;
    for (k=x; k<rb; k++)
        {
        m.background[c] = wrk[k];
        if (m.image[c] != 0)
           wrk[k] = m.image[c];
        c++;
        }
    }

SVGA_DisplayBlock(Buffer , y, y+m.h);

}

void UnDisplayMouse(char *Buffer, int x, int y)
{
int j, k, c=0, bb, rb;
char *wrk;

rb = x+m.w;
if (rb > 639)
   rb = 639;

bb = y+m.h;
if (bb > 479)
   bb = 479;

for (j=y; j<bb; j++)
    {
    wrk = Buffer+stx[j];
    c = (j-y) * m.w;
    for (k=x; k<rb; k++)
        wrk[k] = m.background[c];

    }

}

/***  MISCELLANEOUS FUNCTIONS  ***/

void SetVGAPalette(char *Palette)
{

short int i;
char far *p;

for (i=0; i<768; i++)
    Palette[i] = Palette[i] >> 2;

p = Palette;

outp(0x3c6, 0xff);
for (i=0; i<255; i++)
    {
    outp(0x3c8, i);
    outp(0x3c9, *p++);
    outp(0x3c9, *p++);
    outp(0x3c9, *p++);
    }

return;
}


void RestoreTextMode()
{
    union REGS r;
    r.h.al = 0x03;                    /* 16-colour VGA text mode */
    r.h.ah = 0x00;                    /* Set graphics mode */
    int386(0x10, &r, &r);

}

int MouseInit(void)
{

union REGS r;

r.w.ax = 0x0000;

int386(0x33, &r, &r);

return(r.h.ah);
}

MouseCheck()
{
signed int horiz, vert;

union REGS r;

m.px = m.mx;
m.py = m.my;

r.w.ax = 0x0003;                   /* Get current x/y position and */
int386(0x33, &r, &r);               /* button status */

m.lmb = r.h.bl & 1;                /* LMB currently down */
m.rmb = r.h.bl & 2;                /* RMB currently down */

/*  Current mouse position in SVGA modes is worked out by applying the
    movement value, in Mickeys, from the last position.  */

r.w.ax = 11;
int386(0x33, &r, &r);

m.mx += r.w.cx;          // For a high-resolution mouse, try reducing CX and
m.my += r.w.dx;          // DX before applying them - eg, halve them.

if (m.mx > 639)
   m.mx = 639;

if (m.my > 479)
   m.my = 479;

if (m.mx < 0)
   m.mx = 0;

if (m.my < 0)
   m.my = 0;

r.w.ax = 0x0006;                  /* Get left button release status */
r.w.bx = 0;
int386(0x33, &r, &r);

m.lmr = r.h.bl;

r.w.ax = 0x0006;                  /* Get right button release status */
r.w.bx = 1;
int386(0x33, &r, &r);

m.rmr = r.h.bl;

}

void MouseImageInit(void)
{

/* This function just provides an image for the mouse cursor.  Obviously, it
   would be better to create this image with a paint package and read it in,
   but this will serve as an example */

m.image = image;     // This assigns the pointer to the data block set up at
                     // the top of the program.
m.w = 10;
m.h = 16;
m.background = malloc(160);

}

void LoopCheck(char *Buffer)
{
int dh;

MouseCheck();

if (m.mx != m.px  ||  m.my != m.py)
   {
   UnDisplayMouse(Buffer, m.px, m.py);

   if (m.py > m.my)
      {
      dh = m.py - m.my;
      if (dh > m.h)
         dh = m.h;
      SVGA_DisplayBlock(Buffer, m.py+(m.h-dh), m.py+m.h);
      }

   if (m.py < m.my)
      {
      dh = m.my - m.py;
      if (dh > m.h)
         dh = m.h;
      SVGA_DisplayBlock(Buffer, m.py, m.py+dh);
      }

   DisplayMouse(Buffer, m.mx, m.my);
   }
}
