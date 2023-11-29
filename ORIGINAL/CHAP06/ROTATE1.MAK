.c.obj:
       wcc386 -w4 -4r -mf -oneatx $[*

rotate1.exe : rmain.obj rotate1.obj
	wlink n rotate1 f rmain f rotate1

rmain.obj : rmain.c

rotate1.obj : rotate1.c
