#ifndef __PALETTE_H
#define __PALETTE_H

#pragma library(utils);

#pragma pack(1);

typedef struct {
	unsigned char r,g,b;
} RGB;

#pragma pack;

#ifdef __cplusplus
extern "C" {
#endif

void cdecl gSetDACRange(int start, int len, RGB *palette);
void cdecl gSetDAC(char index, char r, char g, char b);

#ifdef __cplusplus
}
#endif

#endif
