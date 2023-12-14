//
// rotate.cpp
//
#include "lib/retro.h"
#include "lib/retromain.h"

#define XCENTER	(RETRO_WIDTH / 2)
#define YCENTER	(RETRO_HEIGHT / 2)

#define TEXTURE_WIDTH	32
#define TEXTURE_HEIGHT	32

#define ANGLE_360		256
#define ANGLE_180		(ANGLE_360 / 2)
#define ANGLE_90		(ANGLE_360 / 4)
#define ANGLE_MASK		(ANGLE_360 - 1)

#define SIN(x)			sinTable[(x) & ANGLE_MASK]
#define COS(x)			sinTable[((x) + ANGLE_90) & ANGLE_MASK]

float sinTable[ANGLE_360];

void RotateBlit(unsigned char *image, unsigned char *dest, int angle, float scale)
{
	// calculate deltas
	float duCol = COS(angle);
	float dvCol = SIN(angle);
	duCol = duCol * scale;
	dvCol = dvCol * scale;

	float aspectAdjust = 6 / 5;
	float duRow = -dvCol * aspectAdjust;
	float dvRow = duCol * aspectAdjust;

	// center of 32x32 bitmap
	float startingU = 16;
	float startingV = 16;
	startingU -= XCENTER * duCol + YCENTER * duRow;
	startingV -= XCENTER * dvCol + YCENTER * dvRow;

	float rowU = startingU;
	float rowV = startingV;

	for (int y = 0; y < RETRO_HEIGHT; y++) {
		float u = rowU;
		float v = rowV;

		for (int x = 0; x < RETRO_WIDTH; x++) {
			*dest++ = image[((int)(u) & (TEXTURE_WIDTH - 1)) + ((int)(v) & (TEXTURE_WIDTH - 1)) * TEXTURE_WIDTH];
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
	float scale = sinTable[angle] / 2 + 0.5;

	unsigned char *image = RETRO_ImageData();
	unsigned char *dest = RETRO_FrameBuffer();

	// Draw texture
	RotateBlit(image, dest, angle, scale);
}

void DEMO_Initialize(void)
{
	RETRO_LoadImage("assets/rocket.pcx", true);

	for (int i = 0; i < ANGLE_180; i++) {
		sinTable[i] = sin(i * M_PI / ANGLE_180);
	}

	for (int i = ANGLE_180; i < ANGLE_360; i++) {
		sinTable[i] = -sinTable[i - ANGLE_180];
	}
}
