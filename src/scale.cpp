//
// rotate.cpp
//
#include "lib/retro.h"
#include "lib/retromain.h"

#define TEXTURE_WIDTH	32
#define TEXTURE_HEIGHT	32

void ScaleBlit(unsigned char *image, unsigned char *dest, int x0, int y0, int x1, int y1)
{
	for (int x = x0; x < x1; x++) {
		for (int y = y0; y < y1; y++) {
			int u = TEXTURE_WIDTH * (x - x0) / (x1 - x0);
			int v = TEXTURE_HEIGHT * (y - y0) / (y1 - y0);
			dest[x + y * RETRO_WIDTH] = image[u + v * TEXTURE_WIDTH];
		}
	}
}

void DEMO_Render(double deltatime)
{
	unsigned char *image = RETRO_ImageData();
	unsigned char *dest = RETRO_FrameBuffer();

	// Draw texture
	static int xscale = 2;
	static int yscale = 2;

	if (yscale < RETRO_HEIGHT && xscale < RETRO_WIDTH) {
		ScaleBlit(image, dest, 160 - xscale / 2, 100 - yscale / 2, 160 + xscale / 2, 100 + yscale / 2);
		xscale += 2;
		yscale += 2;
	} else {
		xscale = 2;
		yscale = 2;
	}
}

void DEMO_Initialize(void)
{
	RETRO_LoadImage("assets/rocket.pcx", true);
}
