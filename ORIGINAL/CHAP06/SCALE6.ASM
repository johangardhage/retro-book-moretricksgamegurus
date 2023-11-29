                IDEAL
	        P386

DEST_WIDTH	EQU	320
DEST_HEIGHT 	EQU	200
SOURCE_WIDTH	EQU	32
SOURCE_WBITS	EQU	5
SOURCE_HEIGHT	EQU	32
SCREEN		EQU	0A0000h
FIXUP		EQU	12345678h

	MODEL	FLAT, C

	DATASEG

	CODESEG

PROC    C ScaleBlit
	ARG bitmap:PTR BYTE, x0:DWORD, y0:DWORD, x1:DWORD, y1:DWORD

	LOCAL	dv:DWORD, y:DWORD, v:DWORD
	LOCAL	destWidth:DWORD, destHeight:DWORD

	PUBLIC ScaleBlit

	USES	esi,edi

	mov	eax,SOURCE_WIDTH SHL 16
	mov	ebx,[x1]
	sub	ebx,[x0]
	sub	edx,edx
	idiv	ebx

	sub	edx,edx
	shld	edx,eax,16
	shl	eax,16
	mov	[DWORD PTR @@fduWhole1-4],edx
	mov	[DWORD PTR @@fduFract1-4],eax
	mov	[DWORD PTR @@fduWhole2-4],edx
	mov	[DWORD PTR @@fduFract2-4],eax
	shr	ebx,1
	mov	[destWidth],ebx

	; dv = (height << 16) / (y1 - y0)
	mov	eax,SOURCE_HEIGHT SHL 16
	mov	ebx,[y1]
	sub	ebx,[y0]
	sub	edx,edx
	idiv	ebx
	mov	[dv],eax
	mov	[destHeight],ebx

	mov	esi,[bitmap]

	sub	eax,eax
	mov	[v],eax

	; destRow = screen + 320 * y0 + x0
	mov	edi,[y0]
	shl	edi,2
	add	edi,[y0]
	shl	edi,6
	add	edi,[x0]
	add	edi,SCREEN

	mov	edx,[destHeight]

@@rowLoop:

	mov	esi,[v]
	shr	esi,16
	shl	esi,SOURCE_WBITS
	add	esi,[bitmap]

	mov	ebx,[destWidth]

	push	edi

	sub	ecx,ecx

@@colLoop:

	mov	al,[esi]

	add	ecx,FIXUP
@@fduFract1:
	adc	esi,FIXUP
@@fduWhole1:

	mov	ah,[esi]

	add	ecx,FIXUP
@@fduFract2:
	adc	esi,FIXUP
@@fduWhole2:

	mov	[edi],ax
	add	edi,2

	dec	ebx
	jnz	@@colLoop

	pop	edi

	; v += dv
	mov	eax,[dv]
	add	[v],eax

	; destRow += 320
	add	edi,320

	dec	edx
	jnz	@@rowLoop

	ret
ENDP

ENDS

END

