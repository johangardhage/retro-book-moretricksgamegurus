//
// height.cpp
//
#include "lib/retro.h"
#include "lib/retromain.h"

#define RADIX			(32 - kFieldBits)

#define	kFieldBits		9
#define kFieldSize		(1 << kFieldBits)
#define kFieldBytes		(kFieldSize * kFieldSize)

#define TEXTURE_WIDTH	256
#define TEXTURE_HEIGHT	256

#define REND_BITS		8

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
#define PITCH_SPEED		4
#define ROLL_SPEED		32
#define MAX_SPEED		(100 << 8)
#define MAX_ALTITUDE	1000

#define VERT_SCALE		1536
#define DEPTH_CLIP		(256 * VERT_SCALE)

#define SKY_HEIGHT		60				// scale sky height

#define XCENTER			(RETRO_WIDTH / 2)
#define YCENTER			(RETRO_HEIGHT / 2)

#define COPY_SCALE		0x0C400   // factor to scale render buffer (256x256) to screen

#define RENDER_WIDTH	256
#define RENDER_HEIGHT	256

unsigned char			renderbuffer[RENDER_WIDTH * RENDER_HEIGHT];
unsigned char			*heightmap = NULL;
unsigned char			*colormap = NULL;
unsigned char			*cloudmap = NULL;
unsigned char			*background = NULL;

long 					sinTable[ANGLE_360];

void RipHoriz8(int y, int u0, int v0, int u1, int v1, unsigned char *texture)
{
	unsigned char *rendBuffer = renderbuffer + (y << REND_BITS);

	int u = u0 << 16;
	int v = v0 << 16;

	for (int ebp = 0; ebp < RENDER_WIDTH; ebp++) {
		*rendBuffer++ = texture[((u >> (32 - 8)) & (TEXTURE_WIDTH - 1)) + ((v >> (32 - 8)) & (TEXTURE_WIDTH - 1)) * TEXTURE_WIDTH];

		u += (u1 - u0) << (16 - REND_BITS);;
		v += (v1 - v0) << (16 - REND_BITS);;
	}
}

void CopyRotate(unsigned char *bitmap, unsigned char *dest, int angle, long scale)
{
	// calculate deltas
	long duCol = sinTable[(angle + ANGLE_90) & ANGLE_MASK];
	long dvCol = sinTable[angle];
	duCol = (duCol * scale) >> 16;
	dvCol = (dvCol * scale) >> 16;

//	long aspectAdjust = (6 << 16) / 5;
	long aspectAdjust = 0x13333;      // (1.2 << 16), adjust for mode 13h
	long duRow = (-dvCol * aspectAdjust) >> 16;
	long dvRow = (duCol * aspectAdjust) >> 16;

	// center of 256x256 bitmap
	long startingU = 128 << 16;
	long startingV = 128 << 16;
	startingU -= XCENTER * duCol + YCENTER * duRow;
	startingV -= XCENTER * dvCol + YCENTER * dvRow;

	long rowU = startingU;
	long rowV = startingV;

	for (int y = 0; y < RETRO_HEIGHT; y++) {
		long u = rowU;
		long v = rowV;

		for (int x = 0; x < RETRO_WIDTH; x++) {
			*dest++ = bitmap[((u >> 16) & (TEXTURE_WIDTH - 1)) + ((v >> 16) & (TEXTURE_WIDTH - 1)) * TEXTURE_WIDTH];
			u += duCol;
			v += dvCol;
		}

		rowU += duRow;
		rowV += dvRow;
	}
}

void CastRay(int col, int horiz, int altitude, int xorg, int yorg, int dx, int dy)
{
	int dz = (horiz - (RENDER_HEIGHT - 1)) * VERT_SCALE;

	// point to bottom of the column
	int pixel = col + (RENDER_HEIGHT - 1) * RENDER_WIDTH;

	// initial projected pixel height
	int ph = 0;

	// initial ray height
	int z = altitude << 16;

	// initial coordinates
	int x = xorg << (16 - kFieldBits);
	int y = yorg << (16 - kFieldBits);

	while (ph < DEPTH_CLIP) {
		y += dy;
		x += dx;
		z += dz;
		ph += VERT_SCALE;

		// calculate the offset in the height field
		int ypos = WRAP512((y >> (32 - 9))) << 9;
		int xpos = WRAP512(x >> (32 - 9));
		int offset = ypos + xpos;

		int h = heightmap[offset] << 16;

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
	static int xorg = 128L << 16;
	static int yorg = 128L << 16;
	static long altitude = 220;			// arbitrary start height
	static int yaw = ANGLE_90;
	static int roll = 0;
	static int pitch = DEFAULT_PITCH;		// horizon line
	static int skyheight = SKY_HEIGHT;
	static int speed = SPEED * 20;

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

	yaw = (yaw - (SIN(roll) >> 11)) & ANGLE_MASK;
	altitude += (speed * (pitch - DEFAULT_PITCH)) >> 14;
	altitude = MIN(altitude, MAX_ALTITUDE);
	xorg += speed * COS(yaw) >> 8;
	yorg += speed * SIN(yaw) >> 8;

	// Draw texture
	int skyhoriz = pitch + 20;	// overlap clipping of terrain

	// draw the texture mapped sky
	long lsin = sinTable[(yaw + ANGLE_VIEW) & ANGLE_MASK];
	long lcos = sinTable[(yaw + ANGLE_VIEW + ANGLE_90) & ANGLE_MASK];
	long rsin = sinTable[(yaw - ANGLE_VIEW) & ANGLE_MASK];
	long rcos = sinTable[(yaw - ANGLE_VIEW + ANGLE_90) & ANGLE_MASK];

	for (int i = 0; i < skyhoriz && i < RENDER_WIDTH; i++) {
		long dist = (skyheight << 8) / (skyhoriz - i);
		long u0 = (lcos >> 8) * dist + (xorg >> 10);
		long v0 = (lsin >> 8) * dist + (yorg >> 10);
		long u1 = (rcos >> 8) * dist + (xorg >> 10);
		long v1 = (rsin >> 8) * dist + (yorg >> 10);

		RipHoriz8(i, u0, v0, u1, v1, cloudmap);
	}

	for (int col = 0; col < RENDER_WIDTH; col++) {
		int angle = (ANGLE_VIEW * (RENDER_WIDTH - col * 2)) >> REND_BITS;
		long dx = COS(yaw + angle) << (RADIX - 16);
		long dy = SIN(yaw + angle) << (RADIX - 16);

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
		sinTable[i] = sin((double)i * M_PI / ANGLE_180) * 65536;
	}

	for (int i = ANGLE_180; i < ANGLE_360; i++) {
		sinTable[i] = -sinTable[i - ANGLE_180];
	}
}
