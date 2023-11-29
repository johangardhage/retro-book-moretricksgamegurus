/* listing 11.3 */
/*  A function to copy a virtual screen to video ram  */

void copy_vscreen(char far *vscreen);

void copy_vscreen(char far *vscreen)
{
  asm {
    push es        /* save the segment registers */
    push ds
    lds si,[vscreen]   /* get the address of the virtual screen */
    mov ax,0xa000    /* load the address of video ram into the destination */
    mov ds,ax
    xor di,di     /* zero the destination offset */
    mov cx,32000  /* we need to copy 32000 words */
    rep movsw    /* perform the copy */
    pop ds       /* restore the segment registers */
    pop es
  }
}