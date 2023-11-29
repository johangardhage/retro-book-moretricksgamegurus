	        IDEAL
	        P486

; these equates must match their counterparts in the C file

; # bits to represent size of render buffer
REND_SIZEBITS	EQU	8
REND_SIZE	EQU	(1 SHL REND_SIZEBITS)

SCREEN_WIDTH	EQU	240
SCREEN_HEIGHT	EQU	200
SCREEN_PITCH	EQU	320
SCREEN_XCENTER	EQU	120
SCREEN_YCENTER	EQU	100
VIDEO_MEMORY	EQU	0A0000h

; offset at which to copy rotated screen
COPY_OFFSET	EQU	40

; # bits to represent height field coord
FIELD_BITS	EQU	9

; quick triangle calc for view dist
VSCALE		EQU	600h

; clipping distance for rendering
DEPTH_CLIP	EQU	(256 * VSCALE)

FIXUP		EQU	01234567h
ANGLE_360	EQU	1000h
ANGLE_MASK	EQU	(ANGLE_360 - 1)
ANGLE_90	EQU	(ANGLE_360 / 4)

		MODEL	FLAT, C

		DATASEG

		EXTRN C gHeightMap:BYTE
		EXTRN C gColorMap:BYTE
		EXTRN C gAltitude:DWORD
		EXTRN C xOrg:DWORD		; 16:16
		EXTRN C yOrg:DWORD		; 16:16
		EXTRN C gRendBuffer:BYTE
		EXTRN C sinTable:PTR DWORD
		EXTRN C gRollAngle:DWORD

		CODESEG

aspectAdjust	DD	13333h	; (1.2 << 16), adjust for mode 13h

; factor to scale render buffer (256x256) to screen
copyScale	DD	0C400h

dXTemp		DD	?
dYTemp		DD	?
slopeTemp	DD	?
pixel		DD	?


;******************************************************************
;* void cdecl CastRay(int col, int horiz, int deltaX, int deltaY)
;*
;* This is the core rendering code for the height mapping (voxel)
;* engine.  It casts a ray out from the observer, moving at an
;* angle specified by (deltaX, deltaY) through the height field,
;* and at an initial slope determined by gAltitude.  Once an
;* intersection with the height field occurs, a column of pixels
;* is drawn to represent the difference between the ray intersect
;* height and the height of the voxel.  The slope of the ray is
;* then adjusted so that it is tangent to the top of the voxel,
;* and the process repeated.  The function bails out after the
;* projected pixel height reaches DEPTH_CLIP.
;*
;* Whenever you see a label of the form @@f____, it's a self-mod
;* fixup location.
;******************************************************************

PROC C CastRay col:DWORD, horiz:DWORD, deltaX:DWORD, deltaY:DWORD
		PUBLIC	CastRay

		USES	esi,edi,ebp

		mov	eax,[deltaX]
		mov	[dXtemp],eax

		mov	eax,[deltaY]
		mov	[dYtemp],eax

		mov     eax,[horiz]
		sub	eax,(REND_SIZE - 1)
		imul	eax,VSCALE
		mov	[slopeTemp],eax

; point to bottom of the column
		mov	eax,[col]
		add	eax,(REND_SIZE - 1) * REND_SIZE
		mov	[pixel],eax

; initial ray height
		mov	edx,[gAltitude]
		shl	edx,16

; initial coordinates
		mov	esi,[xOrg]
		shl	esi,16 - FIELD_BITS
		mov	edi,[yOrg]
		shl	edi,16 - FIELD_BITS

; do fixups for vector deltas
		mov	eax,[dXtemp]
		mov	[DWORD PTR @@fdX-4],eax

		mov	eax,[dYtemp]
		mov	[DWORD PTR @@fdY-4],eax

		mov	ebp,[slopeTemp]
		xor	eax,eax

; initial projected pixel height offset by DEPTH_CLIP
		mov	ecx,-DEPTH_CLIP

		jmp	$+2		; clear prefetch queue

@@scanLoop:

		add	edi,FIXUP		; next y coordinate
@@fdY:
		add	esi,FIXUP		; next x coordinate
@@fdX:
		add	edx,ebp			; next z coordinate

; calculate the offset in the height field

		mov	ebx,edi
		shr	ebx,32-FIELD_BITS
		shld	ebx,esi,FIELD_BITS
		mov	al,[gHeightMap+ebx]	; get the height

		add	ecx,VSCALE		; next pixel height
		jc	@@done

		shl	eax,16
		cmp	eax,edx			; below ray z?
		jl	@@scanLoop

		; an intersection occured
		mov	al,[gColorMap+ebx]	; get the color
		mov	ebx,[pixel]

@@drawLoop:
		add	ebp,VSCALE		; increment slope
		lea	edx,[edx + ecx + DEPTH_CLIP];	; z += ph
		mov	[gRendBuffer+ebx],al	; draw the pixel
		sub	ebx,REND_SIZE		; move up one line
		jb	@@done
		cmp	edx,eax			; z < h[u,v]?
		jl	@@drawLoop
		mov	[pixel],ebx
		jmp	@@scanLoop

@@done:
		ret
ENDP


;******************************************************************
;* void cdecl CopyRotate(void)
;*
;* This function copies the render buffer to the screen, and
;* rotates and scales it in the process.  This is an adaptation of
;* my spin code.
;*
;* The render buffer must be sized to a power of two, specified in
;* REND_SIZEBITS.  Theoretically, the buffer doesn't have to be
;* square, but it makes things a bit simpler.  The transformation
;* uses a constant specified in the DWORD copyScale, which is tuned
;* to give the best resolution without overflow, i.e. no matter
;* what angle the buffer is rotated, you won't see the edges.
;*
;* The rotation occurs around the center of the screen and the
;* center of the buffer
;******************************************************************
PROC C CopyRotate
		PUBLIC	CopyRotate

		LOCAL startX:DWORD, startY:DWORD
		LOCAL dXCol:DWORD, dYCol:DWORD
		LOCAL dXRow:DWORD, dYRow:DWORD

		USES	esi,edi,ebp

; calculate horizontal deltas

		mov	eax,[gRollAngle]
		and	eax,ANGLE_MASK
		mov	eax,[sinTable + eax * 4]
		imul	[copyScale]
		shrd	eax,edx,REND_SIZEBITS
		mov	[dYCol],eax

		mov	eax,[gRollAngle]
		add	eax,ANGLE_90
		and	eax,ANGLE_MASK
		mov	eax,[sinTable + eax * 4]
		imul	[copyScale]
		shrd	eax,edx,REND_SIZEBITS
		mov	[dXCol],eax

; calculate vertical deltas

		mov	eax,[dYCol]
		neg	eax
		imul	[aspectAdjust]
		shrd	eax,edx,16
		mov	[dXRow],eax

		mov	eax,[dXCol]
		imul	[aspectAdjust]
		shrd	eax,edx,16
		mov	[dYRow],eax

		mov	eax,1 SHL 31
		mov	[startX],eax
		mov	eax,1 SHL 31
		mov	[startY],eax

; move up to top from center

		mov	eax,[dXRow]
		imul	eax,(SCREEN_YCENTER)
		sub	[startX],eax
		mov	eax,[dYRow]
		imul	eax,(SCREEN_YCENTER)
		sub	[startY],eax

; move to left edge from center and fixup column deltas

		mov	eax,[dXCol]
		mov	[DWORD PTR @@fdXCol1-4],eax
		mov	[DWORD PTR @@fdXCol2-4],eax
		mov	[DWORD PTR @@fdXCol3-4],eax
		mov	[DWORD PTR @@fdXCol4-4],eax
		imul	eax,(SCREEN_XCENTER)
		sub	[startX],eax

		mov	eax,[dYCol]
		mov	[DWORD PTR @@fdYCol1-4],eax
		mov	[DWORD PTR @@fdYCol2-4],eax
		mov	[DWORD PTR @@fdYCol3-4],eax
		mov	[DWORD PTR @@fdYCol4-4],eax
		imul	eax,(SCREEN_XCENTER)
		sub	[startY],eax

; fixup end of row deltas

		mov	eax,[dXCol]
		imul	eax,SCREEN_WIDTH
		neg	eax
		add	eax,[dXRow]
		mov	[DWORD PTR @@fdXRow-4],eax

		mov	eax,[dYCol]
		imul	eax,SCREEN_WIDTH
		neg	eax
		add	eax,[dYRow]
		mov	[DWORD PTR @@fdYRow-4],eax

		mov	edi,VIDEO_MEMORY + COPY_OFFSET

		mov	ecx,[startX]
		mov	edx,[startY]

; initialize row count
		mov	esi,SCREEN_HEIGHT

; clear prefetch queue
		jmp	$+2

@@rowloop:
; initialize column count
		mov	ebp,SCREEN_WIDTH / 4

@@colloop:
		mov	ebx,edx
		shr	ebx,32 - REND_SIZEBITS
		shld	ebx,ecx,REND_SIZEBITS
		add	edx,FIXUP
@@fdYCol1:
		add	ecx,FIXUP
@@fdXCol1:
		mov	al,[gRendBuffer+ebx]

		mov	ebx,edx
		shr	ebx,32 - REND_SIZEBITS
		shld	ebx,ecx,REND_SIZEBITS
		add	edx,FIXUP
@@fdYCol2:
		add	ecx,FIXUP
@@fdXCol2:
		mov	ah,[gRendBuffer+ebx]

		bswap	eax

		mov	ebx,edx
		shr	ebx,32 - REND_SIZEBITS
		shld	ebx,ecx,REND_SIZEBITS
		add	edx,FIXUP
@@fdYCol3:
		add	ecx,FIXUP
@@fdXCol3:
		mov	ah,[gRendBuffer+ebx]

		mov	ebx,edx
		shr	ebx,32 - REND_SIZEBITS
		shld	ebx,ecx,REND_SIZEBITS
		add	edx,FIXUP
@@fdYCol4:
		add	ecx,FIXUP
@@fdXCol4:
		mov	al,[gRendBuffer+ebx]

		bswap	eax

		mov	[edi],eax
		add	edi,4
		dec	ebp
		jnz	@@colloop

		add	ecx,FIXUP
@@fdXRow:
		add	edx,FIXUP
@@fdYRow:

; address start of next row
		add	edi,SCREEN_PITCH - SCREEN_WIDTH

; decrement row count
		dec	esi
		jnz	@@rowloop

		ret

ENDP		CopyRotate



PROC C RipHoriz8 y:DWORD, u0:DWORD, v0:DWORD, u1:DWORD, v1:DWORD, texture:PTR BYTE
		PUBLIC	RipHoriz8

		USES	esi,edi,ebp

		mov	eax,[u1]		; 24:8
		sub	eax,[u0]		; 24:8
		shl	eax,16 - REND_SIZEBITS	; 8:24
		mov	[DWORD PTR @@fdu1-4],eax
		mov	[DWORD PTR @@fdu2-4],eax
		mov	[DWORD PTR @@fdu3-4],eax
		mov	[DWORD PTR @@fdu4-4],eax

		mov	eax,[v1]
		sub	eax,[v0]
		shl	eax,16 - REND_SIZEBITS	; 8:24
		mov	[DWORD PTR @@fdv1-4],eax
		mov	[DWORD PTR @@fdv2-4],eax
		mov	[DWORD PTR @@fdv3-4],eax
		mov	[DWORD PTR @@fdv4-4],eax

		mov	edi,OFFSET gRendBuffer
		mov	ebx,[y]
		shl	ebx,REND_SIZEBITS
		add	edi,ebx

		mov	esi,[texture]

		mov	ecx,[u0]		; 24:8
		shl	ecx,16			; 8:24
		mov	edx,[v0]		; 24:8
		shl	edx,16			; 8:24

		mov	ebp,REND_SIZE / 4

; clear prefetch queue
		jmp	$+2

@@colLoop:
		mov	ebx,edx
		shr	ebx,32 - 8
		shld	ebx,ecx,8
		add	edx,FIXUP
@@fdv1:
		add	ecx,FIXUP
@@fdu1:
		mov	al,[esi+ebx]

		mov	ebx,edx
		shr	ebx,32 - 8
		shld	ebx,ecx,8
		add	edx,FIXUP
@@fdv2:
		add	ecx,FIXUP
@@fdu2:
		mov	ah,[esi+ebx]

		bswap	eax

		mov	ebx,edx
		shr	ebx,32 - 8
		shld	ebx,ecx,8
		add	edx,FIXUP
@@fdv3:
		add	ecx,FIXUP
@@fdu3:
		mov	ah,[esi+ebx]

		mov	ebx,edx
		shr	ebx,32 - 8
		shld	ebx,ecx,8
		add	edx,FIXUP
@@fdv4:
		add	ecx,FIXUP
@@fdu4:
		mov	al,[esi+ebx]

		bswap	eax

		mov	[edi],eax
		add	edi,4
		dec	ebp
		jnz	@@colLoop

		ret
ENDP


ENDS

END
