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
#define YAW_SPEED		(ANGLE_360 / 256)
#define MAX_SPEED		(100 << 8)
#define MAX_ALTITUDE	1000

#define VERT_SCALE		1536
#define DEPTH_CLIP		(256 * VERT_SCALE)

#define RENDER_WIDTH	320
#define RENDER_HEIGHT	200

unsigned char 			renderbuffer[RENDER_WIDTH * RENDER_HEIGHT];
unsigned char			*heightmap = NULL;
unsigned char			*colormap = NULL;

long 					sinTable[ANGLE_360];

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
	static int pitch = DEFAULT_PITCH;		// horizon line
	static int speed = SPEED * 20;

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

	if (RETRO_KeyState(SDL_SCANCODE_LEFT)) {
		yaw = (yaw + YAW_SPEED) & ANGLE_MASK;
	} else if (RETRO_KeyState(SDL_SCANCODE_RIGHT)) {
		yaw = (yaw - YAW_SPEED) & ANGLE_MASK;
	}

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

	altitude += (speed * (pitch - DEFAULT_PITCH)) >> 14;
	altitude = MIN(altitude, MAX_ALTITUDE);
	xorg += speed * COS(yaw) >> 8;
	yorg += speed * SIN(yaw) >> 8;

	int skyhoriz = pitch + 100;	// overlap clipping of terrain

	unsigned char *dest = renderbuffer;

	// draw the gradient sky
	for (int i = 0; i < skyhoriz && i < RENDER_HEIGHT; i++) {
		memset(dest, 194 + (skyhoriz - i) / 4, RENDER_WIDTH);
		dest += RENDER_WIDTH;
	}

	for (int col = 0; col < RENDER_WIDTH; col++) {
		int angle = (ANGLE_VIEW * (RENDER_WIDTH - col * 2)) / RENDER_WIDTH;
		long dx = COS(yaw + angle) << (RADIX - 16);
		long dy = SIN(yaw + angle) << (RADIX - 16);

		// the -40 forces the view down to see more of the terrain
		CastRay(col, pitch - 40, altitude, xorg, yorg, dx, dy);
	}

	// copy the buffer to the screen
	memcpy(RETRO.framebuffer, renderbuffer, RENDER_WIDTH * RENDER_HEIGHT);
}

void DEMO_Initialize(void)
{
	colormap = RETRO_LoadImage("assets/democ.pcx", true);
	heightmap = RETRO_LoadImage("assets/demoh.pcx");

	for (int i = 0; i < ANGLE_180; i++) {
		sinTable[i] = sin((double)i * M_PI / ANGLE_180) * 65536;
	}

	for (int i = ANGLE_180; i < ANGLE_360; i++) {
		sinTable[i] = -sinTable[i - ANGLE_180];
	}
}
