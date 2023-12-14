//
// height.cpp
//
#include "lib/retro.h"
#include "lib/retromain.h"

#define TEXTURE_WIDTH	256
#define TEXTURE_HEIGHT	256

#define ANGLE_360		4096
#define ANGLE_180		(ANGLE_360 / 2)
#define ANGLE_90		(ANGLE_360 / 4)
#define ANGLE_60		(ANGLE_360 / 6)
#define ANGLE_45		(ANGLE_360 / 8)
#define ANGLE_30		(ANGLE_360 / 12)
#define ANGLE_MASK		(ANGLE_360 - 1)
#define ANGLE_VIEW   	(ANGLE_60 / 2)	// half view cone angle

#define SIN(x)			sinTable[(x) & ANGLE_MASK]
#define COS(x)			sinTable[((x) + ANGLE_90) & ANGLE_MASK]

#define DEFAULT_PITCH	(RENDER_HEIGHT / 2)

#define SPEED			16
#define PITCH_SPEED		1
#define ROLL_SPEED		32
#define MAX_SPEED		(100 << 8)
#define MAX_ALTITUDE	1000

#define VERT_SCALE		(1536.0 / 65536)
#define DEPTH_CLIP		(256 * VERT_SCALE)

#define SKY_HEIGHT		60				// scale sky height

#define XCENTER			(RETRO_WIDTH / 2)
#define YCENTER			(RETRO_HEIGHT / 2)

#define COPY_SCALE		(50176.0 / 65536)   // factor to scale render buffer (256x256) to screen

#define RENDER_WIDTH	256
#define RENDER_HEIGHT	256

unsigned char			renderbuffer[RENDER_WIDTH * RENDER_HEIGHT];
unsigned char			*heightmap = NULL;
unsigned char			*colormap = NULL;
unsigned char			*cloudmap = NULL;
unsigned char			*background = NULL;

float 					sinTable[ANGLE_360];

void RipHoriz8(int y, float u0, float v0, float u1, float v1, unsigned char *texture)
{
	unsigned char *rendBuffer = renderbuffer + (y * TEXTURE_WIDTH);

	float u = u0;
	float v = v0;

	for (int ebp = 0; ebp < RENDER_WIDTH; ebp++) {
		*rendBuffer++ = texture[((int)u & (TEXTURE_WIDTH - 1)) + ((int)v & (TEXTURE_WIDTH - 1)) * TEXTURE_WIDTH];

		u += (u1 - u0) / TEXTURE_WIDTH;
		v += (v1 - v0) / TEXTURE_WIDTH;
	}
}

void CopyRotate(unsigned char *image, unsigned char *dest, int angle, float scale)
{
	// calculate deltas
	float duCol = COS(angle) * scale;
	float dvCol = SIN(angle) * scale;

	float aspectAdjust = 6 / 5;
	float duRow = -dvCol * aspectAdjust;
	float dvRow = duCol * aspectAdjust;

	// center of 256x256 bitmap
	float startingU = 128;
	float startingV = 128;
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

void CastRay(int col, int horiz, float altitude, float xorg, float yorg, float dx, float dy)
{
	float dz = (horiz - (RENDER_HEIGHT - 1)) * VERT_SCALE;

	// point to bottom of the column
	int pixel = col + (RENDER_HEIGHT - 1) * RENDER_WIDTH;

	// initial projected pixel height
	float ph = 0;

	// initial ray height
	float z = altitude;

	// initial coordinates
	float x = xorg;
	float y = yorg;

	while (ph < DEPTH_CLIP) {
		y += dy;
		x += dx;
		z += dz;
		ph += VERT_SCALE;

		// calculate the offset in the height field
		int ypos = WRAP512(y) * 512;
		int xpos = WRAP512(x);
		int offset = ypos + xpos;

		float h = heightmap[offset];

		// an intersection occured
		if (h > z) {
			unsigned char c = colormap[offset];
			do {
				// increment slope
				dz += VERT_SCALE;
				renderbuffer[pixel] = c;	// draw the pixel
				z += ph;
				pixel -= RENDER_WIDTH;	// move up one line
				if (pixel < 0) {
					return;
				}
			} while (h > z);
		}
	}
}

void DEMO_Render(double deltatime)
{
	static float xorg = 128;
	static float yorg = 128;
	static long altitude = 220;			// arbitrary start height
	static float yaw = ANGLE_90;
	static int roll = 0;
	static int pitch = DEFAULT_PITCH;		// horizon line
	static int skyheight = SKY_HEIGHT;
	static int speed = SPEED;

	if (RETRO_KeyState(SDL_SCANCODE_PAGEUP)) {
		speed += SPEED;
	}
	if (RETRO_KeyState(SDL_SCANCODE_PAGEDOWN)) {
		speed -= SPEED;
	}
	if (speed < -MAX_SPEED) {
		speed = -MAX_SPEED;
	}
	if (speed > MAX_SPEED) {
		speed = MAX_SPEED;
	}

	if (RETRO_KeyState(SDL_SCANCODE_UP) || RETRO_KeyState(SDL_SCANCODE_DOWN)) {
		if (RETRO_KeyState(SDL_SCANCODE_UP)) {
			pitch -= PITCH_SPEED;
		}

		if (RETRO_KeyState(SDL_SCANCODE_DOWN)) {
			pitch += PITCH_SPEED;
		}
	} else {
		if (pitch < DEFAULT_PITCH) {
			pitch++;
		} else if (pitch > DEFAULT_PITCH) {
			pitch--;
		}
	}

	if (RETRO_KeyState(SDL_SCANCODE_LEFT) || RETRO_KeyState(SDL_SCANCODE_RIGHT)) {
		if (RETRO_KeyState(SDL_SCANCODE_LEFT)) {
			roll = (roll - ROLL_SPEED) & ANGLE_MASK;
		}
		if (RETRO_KeyState(SDL_SCANCODE_RIGHT)) {
			roll = (roll + ROLL_SPEED) & ANGLE_MASK;
		}
	} else {
		if (roll > 0 && roll < ANGLE_180) {
			roll = (roll - ROLL_SPEED / 2) & ANGLE_MASK;
		} else if (roll > ANGLE_180 && roll < ANGLE_360) {
			roll = (roll + ROLL_SPEED / 2) & ANGLE_MASK;
		}
	}

	if (RETRO_KeyState(SDL_SCANCODE_COMMA)) {
		skyheight++;
	}
	if (RETRO_KeyState(SDL_SCANCODE_PERIOD)) {
		skyheight--;
	}

	yaw = (int)(yaw - (SIN(roll) * 8)) & ANGLE_MASK;
	altitude += pitch - DEFAULT_PITCH;
	altitude = MIN(altitude, MAX_ALTITUDE);
	xorg += speed / 8 * COS((int)yaw);
	yorg += speed / 8 * SIN((int)yaw);

	// Draw texture
	int skyhoriz = pitch + 20;	// overlap clipping of terrain

	// draw the texture mapped sky
	float lsin = sinTable[((int)yaw + ANGLE_VIEW) & ANGLE_MASK];
	float lcos = sinTable[((int)yaw + ANGLE_VIEW + ANGLE_90) & ANGLE_MASK];
	float rsin = sinTable[((int)yaw - ANGLE_VIEW) & ANGLE_MASK];
	float rcos = sinTable[((int)yaw - ANGLE_VIEW + ANGLE_90) & ANGLE_MASK];

	for (int i = 0; i < skyhoriz && i < RENDER_WIDTH; i++) {
		float dist = (skyheight * 256) / (skyhoriz - i);
		float u0 = lcos * dist + (xorg * 0.5);
		float v0 = lsin * dist + (yorg * 0.5);
		float u1 = rcos * dist + (xorg * 0.5);
		float v1 = rsin * dist + (yorg * 0.5);

		RipHoriz8(i, u0, v0, u1, v1, cloudmap);
	}

	for (int col = 0; col < RENDER_WIDTH; col++) {
		int angle = (ANGLE_VIEW * (RENDER_WIDTH - col * 2)) / RENDER_WIDTH;
		float dx = COS((int)yaw + angle);
		float dy = SIN((int)yaw + angle);

		// the -40 on the pitch forces the view down to see more of the terrain
		CastRay(col, pitch - 40, altitude, xorg, yorg, dx, dy);
	}

	CopyRotate(renderbuffer, RETRO.framebuffer, roll, COPY_SCALE);

	RETRO_BitBlit(background);
}

void DEMO_Initialize(void)
{
	colormap = RETRO_LoadImage("assets/democ.pcx", true);
	heightmap = RETRO_LoadImage("assets/demoh.pcx");
	cloudmap = RETRO_LoadImage("assets/clouds.pcx");
	background = RETRO_LoadImage("assets/backgrnd.pcx");

	for (int i = 0; i < ANGLE_180; i++) {
		sinTable[i] = sin(i * M_PI / ANGLE_180);
	}

	for (int i = ANGLE_180; i < ANGLE_360; i++) {
		sinTable[i] = -sinTable[i - ANGLE_180];
	}
}
