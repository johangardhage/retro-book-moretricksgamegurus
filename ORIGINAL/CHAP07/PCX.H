#ifndef __PCX_H
#define __PCX_H

#pragma library(utils);

#include "palette.h"

enum {
	pcxOkay = 0,
	pcxOpen,		// error opening file
	pcxNotPCX,		// file is not valid PCX format
	pcxNot256,		// file is not 256 color PCX
	pcxNoPalette,	// palette missing from PCX file
	pcxAlloc,		// could not allocate read or decode buffer
	pcxDecode,		// error decoding image data
	pcxSize,		// image is larger than specified range
};

typedef RGB PALETTE[256];

/***********************************************************************
 * ReadPCX256()
 *
 * Read and decode a PCX file
 *
 * Parameters:	fname		filename
 * 				palette		pointer to table of RGB values.  If palette
 * 							is NULL, the image palette will not be
 * 							loaded.
 * 				width		pointer to int to receive image width
 * 				height		pointer to int to receive image height
 * 				image		pointer to pointer to block of memory for
 * 							the decoded image.  If *image is NULL, the
 * 							function will allocate the memory for the
 * 							image.
 *
 * Returns:		pcxOkay on success, enumerated error code on failure
 *
 * Notes:		The function will read images which are no bigger than
 * 				the size specified in *width, *height.  If either
 * 				*width or *height is 0, then the size of the image is
 * 				variable.  If you are passing in a preallocated image
 * 				buffer, you should specify the max size of the image
 * 				with the width and height parameters.  If **image is
 * 				NULL, the file is not decoded.  You can use this
 * 				functionality to read just the size or palette.
 * 				In the case of decoding errors (pcxDecode), you may
 * 				need to free the buffer allocated in *image.
 **********************************************************************/
int ReadPCX256(char *fname, PALETTE *palette, int *width, int *height, char **image);
int WritePCX256(char *fname, PALETTE *palette, int width, int height, char *image);


#endif



