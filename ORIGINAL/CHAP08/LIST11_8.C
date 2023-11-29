/* Listing 11.8 */
/* A sprite Compiler */
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

void main(void)
{
  char infilename[80],outfilename[80],spritename[40];
  FILE *infile,*outfile;
  char xsize,ysize;
  unsigned int offset;
  unsigned char byte1,byte2,x,y;

  printf("\nEnter input file name (sprite file) :");  /* get filenames */
  scanf("%s",infilename);
  printf("\nEnter output file name :");
  scanf("%s",outfilename);
  printf("Enter sprite name (function name) :");
  scanf("%s",spritename);
  printf("\n\n");
				   /*-----------Open files--------*/
  if((infile = fopen(infilename,"rb")) == NULL)
  {
    printf("Could not open input file\n");
    exit(1);
  }
  if((outfile = fopen(outfilename,"wt")) == NULL)
  {
    printf("Could not open output file\n");
    exit(1);
  }
      /*-----Output standard data to output file-----*/
  fprintf(outfile,"void  %s(int x,int y, char far *vscreen)\n{\n",spritename);
  fprintf(outfile,"asm {\npush es\nmov ax,[y]\nmov bx,320\n");
  fprintf(outfile,"mul bx\nadd ax,[x]\nles di,[vscreen]\n");
  fprintf(outfile,"mov di,ax\n\n");

  printf("compiling...\n\n");

  xsize=fgetc(infile);
  ysize=fgetc(infile);
  offset=0;
  for (y=0;y<ysize;y++)
  {
    for(x=0;x<xsize;x+=2)
    {
      byte1=fgetc(infile);  /* get the next two bytes */
      byte2=fgetc(infile);
      if(!byte1 && !byte2)      /* if they are both zero, do nothing */
      {
      }
      if(byte1 && !byte2)    /* if one is not zero, it will be copied */
      {
	fprintf(outfile,"\nmov byte ptr[es:di+%i], 0x%X", offset, byte1);
      }
      if(!byte1 && byte2)   /* dido */
      {
	fprintf(outfile,"\nmov byte ptr[es:di+%i], 0x%X", offset+1, byte2);
      }
      if(byte1 && byte2)  /* neither zero- both copied */
      {
	fprintf(outfile,"\nmov word ptr[es:di+%i], 0x%X%X", offset, byte1, byte2);
      }
      offset+=2;  /*increment offset*/
    }
    offset+=(320-xsize);  /* move to next line below in offset */
  }

  fprintf(outfile,"\npop es\n}\n}\n");

  fclose(infile);  /* clean up and exit */
  fclose(outfile);

  exit(0);
}