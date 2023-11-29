	        IDEAL
	        P386

ANGLE360	EQU	256
ANGLEMASK	EQU	(ANGLE360 - 1)
ANGLE180	EQU	(ANGLE360 / 2)
ANGLE90		EQU	(ANGLE360 / 4)
SCREEN_WIDTH	EQU	320
SCREEN_HEIGHT 	EQU	200
SCREEN_CENTERX	EQU	(SCREEN_WIDTH / 2)
SCREEN_CENTERY	EQU	(SCREEN_HEIGHT / 2)
SOURCE_SIZE	EQU	32
SOURCE_SBITS	EQU	5
SCREEN		EQU	0A0000h
FIXUP		EQU	12345678h

	MODEL	FLAT, C

	DATASEG

	EXTRN	C sinTable:PTR DWORD

aspectAdjust	dd	(6 SHL 16) / 5

	CODESEG

PROC    C RotateBlit
	ARG bitmap:PTR BYTE, angle:DWORD, scale:DWORD

	PUBLIC RotateBlit

	LOCAL startingU:DWORD, startingV:DWORD
	LOCAL dUCol:DWORD, dVCol:DWORD, dURow:DWORD, dVRow:DWORD
	LOCAL rowCount:DWORD

	USES	esi,edi,ebx,ecx

; calculate horizontal deltas

	mov	eax,[angle]
	add	eax,ANGLE90
	and	eax,ANGLEMASK
	mov	eax,[sinTable + eax * 4]
	shl	eax,16 - SOURCE_SBITS
	imul	[scale]
	shrd	eax,edx,16
	mov	[dUCol],eax
	mov	[DWORD PTR @@fdUCol1-4],eax
	mov	[DWORD PTR @@fdUCol2-4],eax
	mov	[DWORD PTR @@fdUCol3-4],eax
	mov	[DWORD PTR @@fdUCol4-4],eax

	mov	eax,[angle]
	and	eax,ANGLEMASK
	mov	eax,[sinTable + eax * 4]
	shl	eax,16 - SOURCE_SBITS
	imul	[scale]
	shrd	eax,edx,16
	mov	[dVCol],eax
	mov	[DWORD PTR @@fdVCol1-4],eax
	mov	[DWORD PTR @@fdVCol2-4],eax
	mov	[DWORD PTR @@fdVCol3-4],eax
	mov	[DWORD PTR @@fdVCol4-4],eax

; calculate vertical deltas

	mov	eax,[dVCol]
	neg	eax
	imul	[aspectAdjust]
	shrd	eax,edx,16
	mov	[dURow],eax

	mov	eax,[dUCol]
	imul	[aspectAdjust]
	shrd	eax,edx,16
	mov	[dVRow],eax

	mov	eax,80000000h
	mov	[startingU],eax
	mov	[startingV],eax

; move up by yOrg

	mov	eax,[dUCol]
	imul    eax,SCREEN_CENTERX
	sub	[startingU],eax

	mov	eax,[dURow]
	imul	eax,SCREEN_CENTERY
	sub	[startingU],eax

	mov	eax,[dVCol]
	imul	eax,SCREEN_CENTERX
	sub	[startingV],eax

	mov	eax,[dVRow]
	imul	eax,SCREEN_CENTERY
	sub	[startingV],eax

; fixup end of row deltas

	mov	eax,[dUCol]
	imul	eax,SCREEN_WIDTH
	neg	eax
	add	eax,[dURow]
	mov	[DWORD PTR @@fdURow-4],eax

	mov	eax,[dVCol]
	imul	eax,SCREEN_WIDTH
	neg	eax
	add	eax,[dVRow]
	mov	[DWORD PTR @@fdVRow-4],eax

	mov	esi,[bitmap]
	mov	edi,SCREEN

	mov	ecx,[startingU]
	mov	edx,[startingV]

	mov	eax,SCREEN_HEIGHT		; initialize row count
	mov	[rowCount],eax

@@rowloop:
	push	ebp
	mov	ebp,SCREEN_WIDTH / 4		; initialize column count

@@colloop:
	mov	ebx,edx
	shr	ebx,32 - SOURCE_SBITS
	shld	ebx,ecx,SOURCE_SBITS
	add	edx,FIXUP
@@fdVCol1:
	add	ecx,FIXUP
@@fdUCol1:
	mov	al,[esi+ebx]

	mov	ebx,edx
	shr	ebx,32 - SOURCE_SBITS
	shld	ebx,ecx,SOURCE_SBITS
	add	edx,FIXUP
@@fdVCol2:
	add	ecx,FIXUP
@@fdUCol2:
	mov	ah,[esi+ebx]

	shl	eax,16

	mov	ebx,edx
	shr	ebx,32 - SOURCE_SBITS
	shld	ebx,ecx,SOURCE_SBITS
	add	edx,FIXUP
@@fdVCol3:
	add	ecx,FIXUP
@@fdUCol3:
	mov	al,[esi+ebx]

	mov	ebx,edx
	shr	ebx,32 - SOURCE_SBITS
	shld	ebx,ecx,SOURCE_SBITS
	add	edx,FIXUP
@@fdVCol4:
	add	ecx,FIXUP
@@fdUCol4:
	mov	ah,[esi+ebx]

	rol     eax,16

	mov	[edi],eax
	add	edi,4
	dec	ebp
	jnz	@@colloop

	add	ecx,FIXUP
@@fdURow:
	add	edx,FIXUP
@@fdVRow:

	pop	ebp
	dec	[rowCount]
	jnz	@@rowloop

	ret

ENDP

ENDS

END
