void main(void)
{
  unsigned char far *vidPtr = (char far *) 0xA0000000L;
  *vidPtr = 0x01c;
}