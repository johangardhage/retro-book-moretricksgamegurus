.c.obj:
       wcc386 -w4 -4r -mf -oneatx $[*

.asm.obj:
        tasm $[* -mx -m2 -q

rotate2.exe : rmain.obj rotate2.obj
	wlink n rotate2 f rmain f rotate2

rmain.obj : rmain.c

rotate2.obj : rotate2.asm
