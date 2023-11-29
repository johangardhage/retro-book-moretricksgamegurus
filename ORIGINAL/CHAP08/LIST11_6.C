/* Listing 11.6 */
/* A function to clear the virtual screen */

void clear_vscreen(char far *vscreen);

void clear_vscreen(char far *vscreen)
{
  asm {
    push es  /* save segment register */
    les di,[vscreen]  /* load the address of the virtual screen */
    mov cx,32000   /* we need to clear 32000 words */
    xor ax,ax    /* clear to zero */
    rep movsw    /* do the clear */
    pop es /* restore segment register */
  }
}