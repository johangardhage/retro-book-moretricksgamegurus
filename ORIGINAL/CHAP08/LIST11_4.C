/* listing 11.4 */
/*  A function to copy a virtual screen to video ram while waiting for vertical retrace */

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

    mov dx,0x03da /* Status register one of the VGA card */
  }
  wait_for_retrace:
  asm {
    in al,dx  /* get the value of status reg 1 */
    test al,0x08  /* test to see if retrace is in progress */
    jnz wait_for_retrace /* if not, keep waiting */

    rep movsw    /* perform the copy */
    pop ds       /* restore the segment registers */
    pop es
  }
}