TITLE Graphix test
DOSSEG
CODESG SEGMENT PARA     'Code'
	ASSUME  CS:CODESG,DS:CODESG,SS:CODESG
	org     100h

Video   proc    near
	mov     ax,13h      ; set video mode 13h
	int     10h         ; call video interrupt

	push    si          ; preserve register contents
	push    di

	mov     ax, 0a000h ; put video segment into ax
	mov     es,ax       ; move into segment register
	mov     ax,0        ; set index to first byte 
	mov     di,ax       ;  in vga segment

	mov     ax,12ch      ; set byte value (arbitrary value)
	mov     cx,64000    ; vga segment is 64k bytes
vLoop:
	mov     es:[di],al  ; write byte at location seg:off
	inc     di          ; increment offset to next location
	loop    vLoop       ; loop to write all 64k bytes

	pop     di          ; restore registers
	pop     si

	mov     ax,03h      ; set video to mode 03h
	int     10h         ;

done:
	mov     ax,04c00h   ; use DOS function
	int     21h         ; call DOS interrupt
	ret
Video   endp
CODESG  ENDS
	end
