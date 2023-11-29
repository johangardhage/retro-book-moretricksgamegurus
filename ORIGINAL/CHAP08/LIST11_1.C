/* Listing 11.1 */
/*  Standard putpixel function without virtual screens  */

void putpixel(int x, int y, char color);


void putpixel(int x, int y, char color)
{
  asm {
    push es
    mov ax,[y]
    mov bx,320
    mul bx
    add ax,[x]
    mov di,ax
    mov ax,0xa000
    mov ds,ax
    mov al,color
    stosb
    pop es
  }
}
