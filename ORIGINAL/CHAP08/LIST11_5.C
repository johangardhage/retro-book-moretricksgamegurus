/* Listing 11.5 */
/*  Putsprite function using virtual screens  */

void vputsprite(int x, int y, char far *sprite, char far *vscreen);


void vputsprite(int x, int y, char far *sprite, char far *vscreen)
{
  asm {
    push es     /* Save the segment registers */
    push ds
    mov ax,[y]  /* calculate the offset of the sprite into video memory */
    mov bx,320
    mul bx
    add ax,[x]
    lds si,[sprite]  /* load the address of the sprite into the source registers */
    les di,[vscreen] /* Load the address of the virtual screen into the dest. */
    mov di,ax        /* put the offset into the destination */
    mov dx,ds:[si]   /* get the x size of the sprite */
    mov cx,ds:[si+1] /* get the y size of the sprite */
    inc si    /* move the source to the beginning of the sprite data */
    inc si
    sub bx,dx  /* create a constant to add to the offset after each x row */
  }               /* is drawn */
  yloop:
  asm {
    push cx    /* save the height left to draw */
    mov cx,dx  /* load the sprite width to use for a loop */
  }
  xloop:
  asm {
    lodsb     /* get the pixel's color */
    cmp al,0     /* is it zero? */
    jz skip_pixel  /* if so, skip it, it is transparent */
    stosb        /* if not zero, draw the pixel */
    loop xloop   /* go to next pixel */
    jmp endx     /* when done with x's for this y coord, jump to end of loop */
  }
  skip_pixel:
  asm {
    inc di   /* if color of pixel was zero, move to next offset */
    loop xloop  /* and go to next pixel */
  }
  endx:
  asm {
    add di,bx  /* add constant to offset to start at next y position */
    pop cx     /* restore the height left to draw */
    loop yloop  /* go to next y positon */
    pop ds     /* restore segments */
    pop es
  }
}
