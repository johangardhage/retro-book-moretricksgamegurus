.c.obj:
       wcc386 -w4 -4r -mf -oneatx $[*

scale1.exe : smain.obj scale1.obj
	wlink n scale1 f smain f scale1

smain.obj : smain.c

scale1.obj : scale1.c
