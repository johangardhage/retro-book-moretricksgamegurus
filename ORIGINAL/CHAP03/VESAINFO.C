/*  VESAINFO.C   -  Find out about your card!  */

#include "vesa.h"
#include <stdio.h>
#include <i86.h>

main()
{

int j;

VESAINFO VESAInfo;
VESAMODEINFO VESAModeInfo;

VESA_GetInfo( &VESAInfo );

printf("Signature:  ");
for (j=0; j<4; j++)
	printf("%c", VESAInfo.Signature[j]);
	
printf("\nVESA Version:  %4x\n", VESAInfo.Version);
printf("Manufacturer:  %s\n", VESAInfo.OEMName);

printf("Capabilities:  ");
for (j=0; j<4; j++)
	printf("%c", VESAInfo.Capabilities[j]);

printf("\nVideo Memory:  %4dK", VESAInfo.VideoMem * 64);

printf("\nSupported Modes:\n");

while (*VESAInfo.VideoMode != 0xFFFF)
	{
	printf("%4x     ", *VESAInfo.VideoMode);
	VESA_GetModeInfo( *VESAInfo.VideoMode++, &VESAModeInfo);
	printf("Gran: %2d    ", VESAModeInfo.Granularity);
	printf("Size: %3d    ", VESAModeInfo.WinSize);
	printf("Bytes:  %4d\n", VESAModeInfo.BytesPerScanLine);
	}

return(0);
}


short int VESA_GetInfo(VESAINFO *Buffer)
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


short int VESA_GetModeInfo( int Mode, VESAMODEINFO *Buffer )
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
