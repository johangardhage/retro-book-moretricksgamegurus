//
//  Standard C include files
//
#include <stdio.h>
#include <stdlib.h>

//
//  Pull in assert() macro
//
#include <assert.h>

//
//  Change these defines for different video modes or window sizes
//
#define SCREEN_MINX 0
#define SCREEN_MINY 0
#define SCREEN_MAXX 319
#define SCREEN_MAXY 199

//
//  This code uses constant sprite sizes
//
#define SPRITE_WIDTH  16
#define SPRITE_HEIGHT 16

//
//  Simple structure for holding sprite data
//
typedef struct
{
  int x,y;		// current sprite location
  char *bitmap;	// pointer to sprite bitmap
} Sprite;

//
//  Stub for BitBlt() function - might be in a third-party graphics
//  library or in custom code.
//
#pragma argsused
void BitBlt(char *bitmap,int x,int y,int w,int h)
{
  if(bitmap == NULL)
  {
    printf("This would have been a hidden bug!\n");
    exit(0);
  }
  else
    printf("BitBlt drew the sprite\n");
}

//
//  Simple sprite drawing function
//
void DrawSprite(Sprite *asprite)
{
//  Make sure a valid pointer was passed
  assert(asprite != NULL);

//  Make sure we point to a valid bitmap
  assert(asprite->bitmap != NULL);

//  Make sure bitmap location is reasonable
  assert(asprite->x >=SCREEN_MINX && 
	asprite->x <=SCREEN_MAXX &&
	asprite->y >=SCREEN_MINY &&
	asprite->y <=SCREEN_MAXY);

// Draw the sprite with clipping
  BitBlt(asprite->bitmap,
	asprite->x,
	asprite->y,
	SPRITE_WIDTH,
	SPRITE_HEIGHT);
}

//
//  Standard C program entry point
//
void main(void)
{
  Sprite theSprite;

  theSprite.bitmap=NULL;
  DrawSprite(&theSprite);
}
