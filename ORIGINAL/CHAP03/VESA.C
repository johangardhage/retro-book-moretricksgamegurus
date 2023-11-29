/*  VESA.C  -  Example 16-bit program to directly update VESA-compliant
               video systems. */

/*  This program will compile under Watcom and Turbo/Borland C, and (probably)
    under others (eg, MS) as well.  You MUST use Large Model and, for Turbo C
    you **MUST** use the '-r-' compile switch (no Register variables)  */

#include <stdlib.h>
#include <dos.h>
#include <malloc.h>
#include <stdio.h>

#ifdef __WATCOMC__
   #include <i86.h>
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

typedef struct {
        char manufacturer;
        char version;
        char encoding;
        char BitsPerPixel;
        int x1, y1;
        int x2, y2;
        int hres;
        int vres;
        char palette[48];
        char reserved;
        char colour_planes;
        int BytesPerLine;
        int PaletteType;
        char filler[58];
} PCXHEADER;

PCXHEADER Header;

int DWidth, DHeight;        // Displayed Height & Width

char palette[768];

main(int argc, char *argv[])
{

FILE *fh;
unsigned int Width, Height, ScreenMode, j;
int hXMS;                   // The handle on our block of XMS memory
char res, *Clr;

VESAINFO VESAInfo;
VESAMODEINFO VESAModeInfo;

if (argc != 2)
	{
	printf("\nUsage:  VESA16 {PCX file name}\n\n");
	exit(0);
	}

if ( (fh = fopen(argv[1], "rb")) == NULL)
	{
	printf("\nFile %s not found\n\n", argv[1]);
	exit(0);
	}

printf("Enter screen mode: (1=640x480, 2=800x600, 3=1024x768, 4=Exit)\n");
res = 0;
while (res < '1' || res >'4')
   res = getch();

switch (res)
   {
   case '1':
   ScreenMode = 0x0101;
   DHeight = 480;
   DWidth = 640;
   break;

   case '2':
   ScreenMode = 0x0103;
   DHeight = 600;
   DWidth = 800;
   break;

   case '3':
   ScreenMode = 0x0105;
   DHeight = 768;
   DWidth = 1024;
   break;

   case '4':
   exit(0);
   }


fread(&Header, sizeof(PCXHEADER), 1, fh);

Width = (Header.x2 - Header.x1) + 1;
Height = (Header.y2 - Header.y1) + 1;

/* Although we COULD unpack direct to the screen in this example, in real life
   we will need to use a buffer in main memory.  To allow large images, we'll
   use XMS memory in the 16-bit implementation - so let's make sure there's
   enough available!  */

if (XMS_Initialise() == -1)
	{
	printf("\nNo XMS driver found - is HIMEM.SYS loaded?\n\n");
	exit(0);
	}

hXMS = XMS_Allocate( (int) ( ( (long)DWidth * DHeight) / 1024) );

if (hXMS == 0 || hXMS == -1)
	{
	printf("\nCould not allocate %3dK of XMS memory\n\n",
              (int) ( ( (long)Width * Height) / 1024) );
	exit(0);
	}

/* Clear the memory out, in case we're displaying, eg., a 640x480 picture
   at 1024x768 - this will stop garbage appearing on screen.  */

Clr = malloc(DWidth);
memset(Clr, 0, DWidth);
for (j=0; j<DHeight; j++)
    XMS_CopyFromDos(Clr, hXMS, (long)DWidth, (long)DWidth*(long)j);

/* OK, let's see if the card is capable of supporting this resolution */

if (VESA_SetMode(ScreenMode) != 0)
	{
	printf("\nYour display card is not VESA-compliant, or its VESA implementation\n");
	printf("does not support a resolution of %4dx%3dx256 colors - try a lower resolution\n\n",
				DWidth, DHeight);
	exit(0);
	}


/* OK, we have a VESA card that can support the resolution */

VESA_GetInfo( &VESAInfo );
VESA_GetModeInfo( ScreenMode, &VESAModeInfo );


/*  OK, everything is in order - unpack the PCX image  */

UnpackPCX(fh, hXMS, Width, Height);

/*  Display the image  */

VESA_UpdateScreen( ScreenMode, &VESAModeInfo, hXMS, DWidth, DHeight);

getch();
RestoreTextMode();
XMS_Dealloc(hXMS);    // This is very important!!

return(0);
}


UnpackPCX(FILE *fh, int hXMS, unsigned int Width,
                          unsigned int Height)
{

/*  Note that this is NOT a properly robust PCX reader - it's for the purposes of
	this example only, and it makes a lot of assumptions, so it's easy to fool  */

char *wbuf, *wptr;
long int Offset;
int j, k;
unsigned char c, l;
int WHeight;

/* Read and set the palette */
fseek(fh, -768L, SEEK_END);
fread(palette, 768, 1, fh);
SetVGAPalette(palette);

/* Reposition to the start of the image data, and continue */
fseek(fh, sizeof(PCXHEADER), SEEK_SET);

wbuf = malloc(Width);
Offset = 0L;

if (DHeight > Height)
   WHeight = Height;
else
   WHeight = DHeight;

for (j=0; j<WHeight; j++)
	{
	wptr = wbuf;
	for (k=0; k<Width;)
		{
		c = fgetc(fh);
		if (c > 191)
			{
			l = c - 192;
			c = fgetc(fh);
			memset(wptr, c, l);
			wptr += l;
			k += l;
			}
		else
			{
			*wptr = c;
			wptr++;   k++;
			}
		}
if (DWidth >= Width)
	  XMS_CopyFromDos(wbuf, hXMS, (long)Width, Offset);
else
   XMS_CopyFromDos(wbuf, hXMS, (long)DWidth, Offset);

	Offset += DWidth;
	}

/*  The above loop decompresses each scan line into a working buffer (wbuf),
    and copies it up into the XMS area after each line has been "un-crunched"
*/

free(wbuf);

return;
}

VESA_UpdateScreen(int ScreenMode, VESAMODEINFO *ModeInfo,
							 int hXMS, int Width, int Height)
{

unsigned long int work1, work2, Offset=0L;
unsigned int NoOfPages, LastPage;
int j;

/* Before we start, work out how many pages are required, and how many bytes
	are needed in the last page.  */

work1 = (long)Width * Height;
NoOfPages = (int) (work1 / 65536L);

work2 = (long)NoOfPages * 65536L;
LastPage = (int) (work1 - work2);

/*  OK, now just update the screen  */
for (j = 0; j < NoOfPages; j++)
	{
	VESA_SetPage(j, ModeInfo->PageSwitchFunction);
	XMS_CopyToDos( MK_FP(0xA000, 0), hXMS, 65536L, Offset);
	Offset += 65536L;
	}

if (LastPage != 0)
	{
	VESA_SetPage(j, ModeInfo->PageSwitchFunction);
	XMS_CopyToDos( MK_FP(0xA000, 0), hXMS, (long)LastPage, Offset);
	}

return;
}

int VESA_SetMode(int Mode)
{

union REGS r;

r.h.ah = 0x4F;
r.h.al = 2;
r.x.bx = Mode;

int86(0x10, &r, &r);


/*  Note - it's not possible to simply return AX (or AH), because AL will
    contain 4Fh after a successful call... */

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

/* Sorry - this function has to be in assembler...  */

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

void XMS_CopyToDos(char *DosPtr, unsigned int hXMS, unsigned long Length,
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

void XMS_CopyFromDos(char *DosPtr, unsigned int hXMS, unsigned long Length,
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

void SetVGAPalette(char *Palette)
{

/* This function sets the palette (will work on any VGA or SVGA card) by
   programming the card directly.  There IS a BIOS function to do it, but
   doing it directly is both quicker and - for protected mode programs - more
   straightforward.  Note that Turbo C reports the outp() lines as having no
   effect - I don't know why it does this (you just try taking 'em away!)  */

short int i;
char far *p;

for (i=0; i<768; i++)
    Palette[i] = Palette[i] >> 2;

p = Palette;

outp(0x3c6, 0xff);
for (i=0; i<=255; i++)
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

