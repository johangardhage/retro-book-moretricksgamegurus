.c.obj:
       wcc386 -w4 -4r -mf -oneatx $[*

scale4.exe : smain.obj scale4.obj
	wlink n scale4 f smain f scale4

smain.obj : smain.c

scale4.obj : scale4.c


