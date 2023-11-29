;////////////////////////////////////////////////////////////////////
;
;  vtools graphics routines for WASM
;
;////////////////////////////////////////////////////////////////////

; _DATA SEGMENT
; define data and variables to be used by vtools routines
_DATA           SEGMENT 'DATA'  USE32
rowBytes        dd      320     ; 320 bytes/row
left            dd      ?       ; left coordinate of rectangle
top             dd      ?       ; top coordinate of rectangle
right           dd      ?       ; right coordinate of rectangle
bottom          dd      ?       ; bottom coordinate of rectangle
bufferWidth     dd      ?       ; calculated buffer width
rowHeight       dd      ?       ; calculated buffer height
_DATA           ENDS

; _TEXT SEGMENT
; code for vtools routines
_TEXT   SEGMENT PUBLIC  'CODE'  USE32

        public  setVideoMode_, viewPort_, writeVideo_, setVGApalette_, waitRetrace_
        
; setVideoMode_ sets the desired video mode
;   according to the value passed in register eax
setVideoMode_   PROC    NEAR
                int     10h             ; call BIOS (video)
                ret
setVideoMode_   ENDP

; viewPort_ displays a regional content of a 320 x 200 pixel
;  buffer to the VGA mode 13h memory
viewPort_     PROC NEAR
                mov     left, eax   ; get bounding rectangle
                mov     top,ebx
                mov     right,ecx
                mov     bottom,edx
                
                sub     ecx, eax     ; compute logical width
                shr     ecx, 2       ; divide by 4
                mov     bufferWidth,ecx ; save buffer width
                
                sub     edx, ebx     ; compute logical row height
                mov     rowHeight, edx ; save row height
                
                mov     eax,ebx      ; get logical top row
                imul    eax,rowBytes ; compute physical row offset
                mov     ebx,eax      ;
                add     ebx, left

                mov     edi,0a0000h  ; initialize edi to video memory
                
                mov     ecx, rowHeight    ; maximum rows to walk   
        rowLoop:
                push    ecx               ; save row loop counter
                mov     ecx, bufferWidth  ; copy only bufferWidth amount of pixels
        bltLoop:
                mov     eax, ds:[esi+ebx] ;transfer buffer contents to eax
                mov     es:[edi+ebx], eax ; copy eax to video memory
                add     edi,4             ; update edi index
                add     esi,4             ; update esi index 
                loop    bltLoop
                pop     ecx               ; restore rowLoop counter
                add     ebx, rowBytes     ; increment the offset to reference the next row
                sub     ebx, right        ; arithmetically compensate for right coordinate offset 
                add     ebx, left         ; add left offset at the start of new row
                loop    rowLoop
                ret
viewPort_     ENDP

; writeVideo_  writes the entire 320 x 200 offsreen buffer to 
;  VGA mode 13h memory
writeVideo_           PROC NEAR
                mov     edi, 0a0000h      ; write buffer [esi] to video memory [edi]
                mov     ecx, 16000        ;  repeat 16000 (64000 bytes / 4 bytes per movsd) times
                rep     movsd             ;  perform 32-bit move string operation
                ret
writeVideo_           ENDP

; write the DAC registers with a VGA a palette
setVGApalette_  PROC NEAR
                cli
                mov  edi, eax
                dec  edi
                mov  ecx, eax
                add  ecx, 767d
                
                mov  dx, 3c8h             ; write port
                xor  al, al
                out  dx, al
                
                mov  dx, 3c9h             ; data port
                
         loopPal:
                inc  edi
                mov  ax, ds:[edi]
                out  dx, al
                inc  edi
                mov  ax, ds:[edi]
                out  dx, al
                inc  edi
                mov  ax, ds:[edi]
                out  dx, al
                
                cmp  edi, ecx
                jne  loopPal
                sti                
                
                ret
setVGApalette_  ENDP

; wait for the vertical retrace to complete
waitRetrace_    PROC NEAR
                mov  dx, 3dah
         vrtOff: 
                in   al, dx
                test al, 8h
                jnz  vrtOff
         vrtResume:
                in   al, dx
                test al, 8h
                jnz  vrtResume
                ret
waitRetrace_    ENDP                
                     
_TEXT           ENDS
                END

