.c.obj:
       wcc386 -w4 -4r -mf -oneatx $[*

scale2.exe : smain.obj scale2.obj
	wlink n scale2 f smain f scale2

smain.obj : smain.c

scale2.obj : scale2.c


