//
// rotate.cpp
//
#include "lib/retro.h"
#include "lib/retromain.h"

#define XCENTER	(RETRO_WIDTH / 2)
#define YCENTER	(RETRO_HEIGHT / 2)

#define TEXTURE_WIDTH 32
#define TEXTURE_HEIGHT 32

#define ANGLE_360		256
#define ANGLE_180		(ANGLE_360 / 2)
#define ANGLE_90		(ANGLE_360 / 4)
#define ANGLE_MASK		(ANGLE_360 - 1)

long sinTable[ANGLE_360];

void RotateBlit(unsigned char *image, unsigned char *dest, int angle, long scale)
{
	// calculate deltas
	long duCol = sinTable[(angle + ANGLE_90) & ANGLE_MASK];
	long dvCol = sinTable[angle];
	duCol = (duCol * scale) >> 16;
	dvCol = (dvCol * scale) >> 16;

	long aspectAdjust = (6 << 16) / 5;
	long duRow = (-dvCol * aspectAdjust) >> 16;
	long dvRow = (duCol * aspectAdjust) >> 16;

	// center of 32x32 bitmap
	long startingU = 16 << 16;
	long startingV = 16 << 16;
	startingU -= XCENTER * duCol + YCENTER * duRow;
	startingV -= XCENTER * dvCol + YCENTER * dvRow;

	long rowU = startingU;
	long rowV = startingV;

	for (int y = 0; y < RETRO_HEIGHT; y++) {
		long u = rowU;
		long v = rowV;

		for (int x = 0; x < RETRO_WIDTH; x++) {
			*dest++ = image[((u >> 16) & (TEXTURE_WIDTH - 1)) + ((v >> 16) & (TEXTURE_WIDTH - 1)) * TEXTURE_WIDTH];
			u += duCol;
			v += dvCol;
		}

		rowU += duRow;
		rowV += dvRow;
	}
}

void DEMO_Render(double deltatime)
{
	// Calculate frame
	static float frame = 0;
	frame += deltatime * 100;

	int angle = WRAP256(frame);
	long scale = sinTable[angle] / 2 + 36864;

	unsigned char *image = RETRO_ImageData();
	unsigned char *dest = RETRO_FrameBuffer();

	// Draw texture
	RotateBlit(image, dest, angle, scale);
}

void DEMO_Initialize(void)
{
	RETRO_LoadImage("assets/rocket.pcx", true);

	for (int i = 0; i < ANGLE_180; i++) {
		sinTable[i] = sin((double)i * M_PI / ANGLE_180) * 65536;
	}

	for (int i = ANGLE_180; i < ANGLE_360; i++) {
		sinTable[i] = -sinTable[i - ANGLE_180];
	}
}
