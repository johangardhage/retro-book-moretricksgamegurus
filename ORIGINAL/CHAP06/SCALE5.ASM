                IDEAL
	        P386

DEST_WIDTH	EQU	320
DEST_HEIGHT 	EQU	200
SOURCE_WIDTH	EQU	32
SOURCE_WBITS	EQU	5
SOURCE_HEIGHT	EQU	32
SCREEN		EQU	0A0000h

	MODEL	FLAT, C

	DATASEG

	CODESEG

PROC    C ScaleBlit
	ARG bitmap:PTR BYTE, x0:DWORD, y0:DWORD, x1:DWORD, y1:DWORD

	LOCAL	du:DWORD, dv:DWORD, rowCount:DWORD
	LOCAL	destWidth:DWORD, destHeight:DWORD

	PUBLIC ScaleBlit

	USES	esi,edi

	; du = (width << 16) / (x1 - x0)
	mov	eax,SOURCE_WIDTH SHL 16
	mov	ebx,[x1]
	sub	ebx,[x0]
	mov	[destWidth],ebx
	sub	edx,edx
	idiv	ebx
	mov	[du],eax

	; dv = (height << 16) / (y1 - y0)
	mov	eax,SOURCE_HEIGHT SHL 16
	mov	ebx,[y1]
	sub	ebx,[y0]
	mov	[destHeight],ebx
	sub	edx,edx
	idiv	ebx
	mov	[dv],eax

	mov	esi,[bitmap]

	; v = 0
	sub	edx,edx

	mov	eax,[destHeight]
	mov	[rowCount],eax

	; destRow = screen + 320 * y0 + x0
	mov	edi,[y0]
	shl	edi,2
	add	edi,[y0]
	shl	edi,6
	add	edi,[x0]
	add	edi,SCREEN

@@rowLoop:

	; u = 0
	sub	ecx,ecx

	; sourceRow = bitmap + (v >> 16) * width;
	mov	esi,edx
	shr	esi,16
	shl	esi,SOURCE_WBITS
	add	esi,[bitmap]

	mov	ebx,[destWidth]
	push	edi

@@colLoop:

	; *destRow++ = sourceRow[u >> 16];
	mov	eax,ecx
	shr	eax,16
	mov	al,[esi+eax]
	mov	[edi],al
	inc	edi

	; u += du
	add	ecx,[du]

	dec	ebx
	jnz	@@colLoop

	pop	edi

	; v += dv
	add	edx,[dv]

	; destRow += 320
	add	edi,320

	dec	[rowCount]
	jnz	@@rowLoop

	ret
ENDP



ENDS

END

