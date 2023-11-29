/* listing 11.9 */
/*  A function to copy a virtual screen to video ram while waiting for vertical retrace */
/*  an using the technique to avoid sprite clipping */

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
    mov si,16     /* start 1 pisels from the side */
    mov cx,192    /* we need to copy 192 rows */

    mov dx,0x03da /* Status register one of the VGA card */
  }
  wait_for_retrace:
  asm {
    in al,dx  /* get the value of status reg 1 */
    test al,0x08  /* test to see if retrace is in progress */
    jnz wait_for_retrace /* if not, keep waiting */
  }
  yloop:
  asm {
    push cx      /* save the row count */
    mov cx,320   /* 320 columns per row */
    rep movsw    /* perform the copy */
    add si,16    /* skip the 16 pixel edge */
    pop cx       /* restore row count */
    loop yloop   /* repeat until finished */
    pop ds       /* restore the segment registers */
    pop es
  }
}