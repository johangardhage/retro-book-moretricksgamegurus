/*  Listing 11-2  */
/*   Putpixel function that uses a virtual screen   */

void vputpixel(int x, int y, char color, char far *vscreen);


void vputpixel(int x, int y, char color, char far *vscreen)
{
  asm {
    push es
    mov ax,[y]
    mov bx,320
    mul bx
    add ax,[x]
    les di,[vscreen]
    mov di,ax
    mov al,color
    stosb
    pop es
  }
}