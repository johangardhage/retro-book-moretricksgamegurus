_mydat		segment para public 'data'
extrn		_flagtable:byte
OldInt9		dw      0000,0000       ;Address of old interrupt 9 handler
_mydat		ends

_mycode		segment para public 'code'
public		_SetKb,_ResetKb
assume		cs:_mycode,ds:_mydat

;This is the new interrupt 09h handler.
int9		proc
		cli				;Disable other interrupts
		push	cx			;We've got to save all registers
		push	bx
		push	ax
		push	ds
		mov	ax,SEG _flagtable	;Set up addressing
		mov	ds,ax			;For the data.

WaitForIt:	in	al,64h			;64h is status port-key done yet?
		test	al,02h			;Check if controller is done
		loopnz	WaitForIt		;Not ready-wait
		in	al,60h			;It's ready-get the code.

;The make or break code is now in al. Remember that make and break codes are
;the same, except for the high bit. So, we can use some shifts to get the high
;bit out, and use it to set our flags.
		xor	ah,ah			;Clear ah first
		shl	ax,1			;ah contains the high bit
		shr	al,1			;al contains just the low bits.
		xor	ah,1			;Invert-Make code should be one.
		xor	bh,bh			;Clear bh
		mov	bl,al			;Load bx with scan code
		mov	_flagtable[bx],ah	;Send it to the table

;Need the following to make keyoard controller happy.
		in	al,61h			;Get control status
		mov	ah,al			;Copy it
		or	al,80h			;Set KB enable bit
		out	61h,al			;Write to control port
		xchg	ah,al			;Send original value
		out	61h,al			;Back again

;Clean up and exit the interrupt.
		mov	al,20h			;Tell interrupt chip
		out	20h,al			;That we're done.

		pop	ds			;That's it.
		pop	ax
		pop	bx
		pop	cx
		sti				;Reenable interrupts.
		iret
int9	endp

;Install the Keyboard interrupt handler-call this at startup
_SetKb		proc	far
		push	ds			;Save registers
		push	es
		push	ax
		push	dx
		mov	al,9h			;Request keyboard interrupt
		mov	ah,35h			;DOS get interrupt vector
		int	21h			;DOS call
		mov	ax,seg OldInt9		;Get the address to save it at.
		mov	ds,ax
		mov	OldInt9[0],bx		;Save offset
		mov	OldInt9[2],es		;Save segment
		mov	ax,seg int9		;Get new segment
		mov	ds,ax
		mov	dx,offset int9		;Get new offset
		mov	al,9h			;Change keyboard interrupt
		mov	ah,25h			;DOS change interrupt vector
		int	21h			;DOS call
		pop	dx
		pop	ax
		pop	es
		pop	ds
		retf
_SetKb		endp

;Remove the Keyboard interrupt handler-call this at shutdown
_ResetKb	proc	far
		push	ds			;Save registers
		push	ax
		push	dx
		mov	ax,seg OldInt9		;Get the address to restore from.
		mov	ds,ax
		mov	dx,OldInt9[0]		;Restore offset
		mov	ax,OldInt9[2]		;Restore segment
		mov	ds,ax
		mov	al,9h			;Change keyboard interrupt
		mov	ah,25h			;DOS change interrupt vector
		int	21h			;DOS call
		pop	dx
		pop	ax
		pop	ds
		retf
_ResetKb	endp
_mycode		ends
ends
end
