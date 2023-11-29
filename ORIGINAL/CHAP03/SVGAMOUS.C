/*  SVGAMOUS.C - 16-bit example implementing a mouse pointer at 640x480x256
                 resolution.  Includes example of direct support for non-VESA
                 card.   */


#include <stdio.h>
#include <stdlib.h>

#ifdef __WATCOMC__
   #include <i86.h>
   #include <malloc.h>
   #define enable _enable
   #define disable _disable
#else
   #include <dos.h>
   #include <alloc.h>
#endif


 /*   Basic VESA structures  */

typedef struct {
   char							Signature[4];
   unsigned short int			Version;
   char 	 					*OEMName;					/* This is a FAR pointer - use Large model! */
   unsigned char				Capabilities[4];
   unsigned short int 	 		*VideoMode;
   short int					VideoMem;
   char							dummy[238];                         /*Pad to 256 bytes */
   } VESAINFO;

typedef struct {
   unsigned short int		ModeAttr;
   unsigned char			WinAAttr;
   unsigned char			WinBAttr;
   unsigned short int		Granularity;
   unsigned short int		WinSize;
   unsigned short int		WinASegment;
   unsigned short int		WinBSegment;
   unsigned short int		(*PageSwitchFunction)();
   unsigned short int		BytesPerScanLine;

        /*  Not all VESA cards provide the following optional information  */
   unsigned int				XResolution;
   unsigned int				YResolution;
   unsigned char			XCharSize;
   unsigned char			YCharSize;
   unsigned char			NumberOfPlanes;
   unsigned char			BitsPerPixel;
   unsigned char			NumberOfBanks;
   unsigned char			MemoryModel;
   unsigned char			BankSize;
   char						dummy[227];                         /*Pad to 256 bytes */
   } VESAMODEINFO;


/*  Information block for XMS memory copies */

typedef struct {
        unsigned long int len;
        unsigned short int  src;
        unsigned long int soff;
        unsigned short int  dh;
        unsigned long int doff;
        } XMSBLOCK;


/* Function protoypes */


int cdecl XMS_Initialise(void);
int cdecl XMS_Allocate(int);
int cdecl XMS_Dealloc(int);
int cdecl XMS_Copy(void *);
void XMS_CopyToDos(char *, unsigned int, unsigned long, unsigned long);
void XMS_CopyFromDos(char *, unsigned int, unsigned long, unsigned long);

long int cdecl XMS_ConvertPtr(char far *ptr);

#ifdef __WATCOMC__
   #pragma aux XMS_CopyToDos   modify [si];
   #pragma aux XMS_CopyFromDos modify [si];
#endif


int VESA_SetMode(int);
int VESA_GetInfo(VESAINFO *);
int VESA_GetModeInfo(int, VESAMODEINFO *);
void VESA_SetPage(int, void *);

void SetVGAPalette(char *);
void RestoreTextMode(void);


#define TRIDENT 1
#define VESA    2
#define HUALON  3

struct {                      //  Mouse info block
   signed int mx, my;         //  Current x/y position
   signed int px, py;         //  Previous x/y position
   char lmb, rmb;             //  Button press status, left and right
   char lmr, rmr;             //  Button release status, left & right
   int h, w;                  //  Height and width of pointer image
   char *image;               //  Pointer to image data
   char *background;          //  Pointer to saved background
   } m;

long stx[480];
char *sts[480];
char stb[480];
char stp[480];

char lastp = 0;

char SVGAcard;
unsigned short int (* VESApointer)();

VESAINFO VESAInfo;
VESAMODEINFO VESAModeInfo;

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
void SVGA_DisplayBlock(unsigned int, int, int);
void SVGA_DisplayLine(unsigned int, unsigned int);
void SVGA_DisplayScreen(unsigned int);
void DisplayMouse(unsigned int, int, int);
void UnDisplayMouse(unsigned int, int, int);
void MouseImageInit(void);
void LoopCheck(unsigned int);

#ifdef __WATCOMC__
   #pragma aux XMS_CopyToDos   modify [si];
   #pragma aux XMS_CopyFromDos modify [si];
#endif

main(int argc, char *argv[])
{
FILE *fp;
unsigned int hXMS, segoff, j;
char  z;
long c1, c2, x2off;
char clr[640];

SVGAcard = VESA;          // A sensible default!

if (argc == 2)
   {
   if (strcmp(argv[1], "TRIDENT") == NULL)
      SVGAcard = TRIDENT;
   if (strcmp(argv[1], "HUALON") == NULL)
      SVGAcard = HUALON;
   }

XMS_Initialise();

if ((hXMS = XMS_Allocate(300)) == NULL)
   exit(0);

for (j=0;j<480;j++)
   {
   memset(clr, (unsigned char)j, 640);
   XMS_CopyFromDos(clr, hXMS, 640L, (long)j*640L);
   }

MouseImageInit();
MouseInit();

SVGA_InitTables();

SVGA_SetMode();       // For this example, 640x480 only

for (j=0; j<480; j++)
   SVGA_DisplayLine(hXMS, j);

m.mx = 0;  m.my = 0;      // Start at the top-left of the screen...
m.px = 0;  m.py = 0;

DisplayMouse(hXMS, m.mx, m.my);

while(!m.lmr)
   {
   LoopCheck(hXMS);
   }

XMS_Dealloc(hXMS);

RestoreTextMode();
}


void SVGA_InitTables(void)
{

unsigned int j;
char page=0;
long runner=0L;

for (j=0; j<480; j++)
    {
    stx[j] = (long)j * 640;
    sts[j] = MK_FP(0xa000, (unsigned int)runner);
    if (runner+640L > 65535L)
       {
       stb[j] = 1;
       runner -= 65536L;
       }
    else
       stb[j] = 0;

    if (j != 0 && stb[j-1] == 1)
       page++;

    stp[j] = page;
    runner += 640L;
    }
}

/***  MODE SELECTION AND PAGE-SWITCHING FUNCTIONS  ***/

void SVGA_SetMode(void)
{

union REGS r;

switch(SVGAcard)
   {
   case TRIDENT:
      r.x.ax = 0x005d;
      int86(0x10, &r, &r);
      break;

   case VESA:
      VESA_SetMode(0x0101);
      VESA_GetInfo(&VESAInfo);
      VESA_GetModeInfo(0x0103, &VESAModeInfo);
      VESApointer = VESAModeInfo.PageSwitchFunction;
      break;

   case HUALON:
      r.x.ax = 0x002d;
      int86(0x10, &r, &r);
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
      disable();
      outp(0x03ce, 0x06);
      i = inp(0x03cf);
      outpw(0x03ce, (( i | 0x04) << 8) | 0x0006);
      outp(0x03c4, 0x0b);
      i=inp(0x03c5);
      outpw(0x03c4, ((n ^ 0x02) << 8) | 0x000e);
      enable();
      break;

   case VESA:
      VESA_SetPage(n, VESApointer);
      break;

   case HUALON:
      n = (n << 12) | 0x00ee;
      outpw(0x03c4, n);
      break;

   }

}


int VESA_SetMode(int Mode)
{

union REGS r;

r.h.ah = 0x4F;
r.h.al = 2;
r.x.bx = Mode;

int86(0x10, &r, &r);

if (r.h.ah == 0)
   return(0);
else
   return(1);
}

int VESA_GetInfo(VESAINFO *Buffer)
{

union REGS r;
struct SREGS sr;

r.x.ax = 0x4F00;
sr.es = FP_SEG(Buffer);
r.x.di = FP_OFF(Buffer);

int86x(0x10, &r, &r, &sr);

if (r.h.al == 0x4F)
   return(0);
else
   return(-1);
}


int VESA_GetModeInfo( int Mode, VESAMODEINFO *Buffer )
{

union REGS r;
struct SREGS sr;

r.x.ax = 0x4F01;
r.x.cx = Mode;
sr.es = FP_SEG(Buffer);
r.x.di = FP_OFF(Buffer);

int86x(0x10, &r, &r, &sr);

if (r.h.al == 0x4F)
   return(0);
else
   return(1);
}


void VESA_SetPage(int Page, void *FunctionPointer)
{

#ifdef __WATCOMC__

   void AsmSetPage(void);

   #pragma aux AsmSetPage = \
      "xor bx, bx"          \
      "mov dx, Page"        \
      "call [FunctionPointer]";

   AsmSetPage();

#else

   asm {
       xor bx, bx;
       mov dx, Page;
       call [FunctionPointer];
       }

#endif
}


/***  XMS FUNCTIONS  ***/

void XMS_CopyToDos(void *DosPtr, unsigned int hXMS, unsigned long Length,
                            unsigned long Offset)
{

XMSBLOCK x;

x.len  = Length;
x.src  = hXMS;
x.soff = Offset;
x.dh   = 0;                       //  An XMS handle of 0 means DOS memory
x.doff = XMS_ConvertPtr(DosPtr);

XMS_Copy(&x);
}

void XMS_CopyFromDos(void *DosPtr, unsigned int hXMS, unsigned long Length,
                            unsigned long Offset)
{

XMSBLOCK x;

x.len  = Length;
x.src  = 0;                       //  An XMS handle of 0 means DOS memory
x.soff = XMS_ConvertPtr(DosPtr);
x.dh   = hXMS;
x.doff = Offset;

XMS_Copy(&x);
}


/***  SCREEN UPDATING FUNCTIONS  ***/

void SVGA_DisplayBlock(unsigned int hXMS, int start, int end)
{
long wrk1, wrk2;
char pag1, pag2;
char far *ScrPtr;
unsigned int Lngth, wrki;
long lgth;

if (start < 0) start = 0;    // Just to protect us from ourselves!
if (end > 479) end = 479;

if (lastp != stp[start])
   SVGA_SetPage(stp[start]);

/*  1.  The entire block is in one page - great!  */
if (stp[start] == stp[end])
   {
   ScrPtr = sts[start];
   Lngth = (int)((stx[end] - stx[start]) + 640L);
   XMS_CopyToDos(ScrPtr, hXMS, (long)Lngth, stx[start]);
   }

else

/*  2.  The block straddles a page boundary - not so great!  */
   {
   if (stp[end] - stp[start] == 1)
      {
      /*  OK, do the start page first... */
      ScrPtr = sts[start];
      Lngth = (int)((65536L * (stp[start]+1L)) - stx[start]);

      XMS_CopyToDos(ScrPtr, hXMS, (long)Lngth, stx[start]);

     /*  Right - swap pages and do the next one...  */

      SVGA_SetPage(stp[start]+1);

      lgth = ((stx[end]-stx[start]) + 640L) - (long)Lngth;
      wrk1 = stx[start] + (long)Lngth;
      Lngth = (int)lgth;

      ScrPtr = MK_FP(0xa000, 0);

      XMS_CopyToDos(ScrPtr, hXMS, (long)Lngth, wrk1);
      }
   else

/*  3.  This is a large block, and multiple page switches are required.
        Update it line-by-line, or the whole screen */
      {
      if (end - start > 150)
         SVGA_DisplayScreen(hXMS);
      else
         for (wrki=start; wrki<= end; wrki++)
             SVGA_DisplayLine(hXMS, wrki);
      }
   }
}


void SVGA_DisplayLine(unsigned int hXMS, unsigned int line)
{

char page, *ScrPointer, odd=0;
long XMSOffset;
long lgth;

page = stp[line];
if (page != lastp)
   SVGA_SetPage(page);

XMSOffset  = stx[line];
ScrPointer = sts[line];

if (stb[line] == 0)
   XMS_CopyToDos(ScrPointer, hXMS, 640L, XMSOffset);

else
   {
   lgth = (65536L * ((long)page+1L)) - XMSOffset;
   XMS_CopyToDos(ScrPointer, hXMS, lgth, XMSOffset);
   SVGA_SetPage(page+1);
   XMSOffset += lgth;
   ScrPointer = MK_FP(0xa000, 0);
   lgth = 640 - lgth;

   XMS_CopyToDos(ScrPointer, hXMS, lgth, XMSOffset);
   }
}


void SVGA_DisplayScreen(unsigned int hXMS)
{

/*  This function is hard-wired to update an entire 640x480 screen  */

char *ptr;

ptr = MK_FP(0xa000, 0);

SVGA_SetPage(0);
XMS_CopyToDos(ptr, hXMS, 65536L, 0L);
SVGA_SetPage(1);
XMS_CopyToDos(ptr, hXMS, 65536L, 65536L);
SVGA_SetPage(2);
XMS_CopyToDos(ptr, hXMS, 65536L, 131072L);
SVGA_SetPage(3);
XMS_CopyToDos(ptr, hXMS, 65536L, 196608L);
SVGA_SetPage(4);
XMS_CopyToDos(ptr, hXMS, 45056L, 262144L);
}

/***  MOUSE CURSOR DISPLAYING FUNCTIONS  ***/

void DisplayMouse(unsigned int hXMS, int x, int y)
{
int j, k, c=0, bb, rb;
char *wrk;

wrk = malloc(640);

rb = x+m.w;
if (rb > 639)
   rb = 639;

bb = y+m.h;
if (bb > 479)
   bb = 479;

for (j=y; j<bb; j++)
    {
    XMS_CopyToDos(wrk, hXMS, 640L, stx[j]);
    c = (j-y) * m.w;
    for (k=x; k<rb; k++)
        {
        m.background[c] = wrk[k];
        if (m.image[c] != 0)
           wrk[k] = m.image[c];
        c++;
        }
    XMS_CopyFromDos(wrk, hXMS, 640L, stx[j]);
    }

SVGA_DisplayBlock(hXMS, y, y+m.h);

free(wrk);

}

void UnDisplayMouse(unsigned int hXMS, int x, int y)
{
int j, k, c=0, bb, rb;
char *wrk;

wrk = malloc(640);

rb = x+m.w;
if (rb > 639)
   rb = 639;

bb = y+m.h;
if (bb > 479)
   bb = 479;

for (j=y; j<bb; j++)
    {
    XMS_CopyToDos(wrk, hXMS, 640L, stx[j]);
    c = (j-y) * m.w;
    for (k=x; k<rb; k++)
        wrk[k] = m.background[c];

    XMS_CopyFromDos(wrk, hXMS, 640L, stx[j]);
    }

free(wrk);

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
    int86(0x10, &r, &r);

}

int MouseInit(void)
{

union REGS r;

r.x.ax = 0x0000;

int86(0x33, &r, &r);

return(r.h.ah);
}

MouseCheck()
{
signed int horiz, vert;

union REGS r;

m.px = m.mx;
m.py = m.my;

r.x.ax = 0x0003;                   /* Get current x/y position and */
int86(0x33, &r, &r);               /* button status */

m.lmb = r.h.bl & 1;                /* LMB currently down */
m.rmb = r.h.bl & 2;                /* RMB currently down */

/*  Current mouse position in SVGA modes is worked out by applying the
    movement value, in Mickeys, from the last position.  */

r.x.ax = 11;
int86(0x33, &r, &r);

m.mx += r.x.cx;          // For a high-resolution mouse, try reducing CX and
m.my += r.x.dx;          // DX before applying them - eg, halve them.

if (m.mx > 639)
   m.mx = 639;

if (m.my > 479)
   m.my = 479;

if (m.mx < 0)
   m.mx = 0;

if (m.my < 0)
   m.my = 0;

r.x.ax = 0x0006;                  /* Get left button release status */
r.x.bx = 0;
int86(0x33, &r, &r);

m.lmr = r.h.bl;

r.x.ax = 0x0006;                  /* Get right button release status */
r.x.bx = 1;
int86(0x33, &r, &r);

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

void LoopCheck(unsigned int hXMS)
{
int dh;

MouseCheck();

if (m.mx != m.px  ||  m.my != m.py)
   {
   UnDisplayMouse(hXMS, m.px, m.py);

   if (m.py > m.my)
      {
      dh = m.py - m.my;
      if (dh > m.h)
         dh = m.h;
      SVGA_DisplayBlock(hXMS, m.py+(m.h-dh), m.py+m.h);
      }

   if (m.py < m.my)
      {
      dh = m.my - m.py;
      if (dh > m.h)
         dh = m.h;
      SVGA_DisplayBlock(hXMS, m.py, m.py+dh);
      }

   DisplayMouse(hXMS, m.mx, m.my);
   }
}
