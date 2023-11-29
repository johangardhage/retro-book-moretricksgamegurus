.c.obj:
       wcc386 -w4 -4r -mf -oneatx $[*

scale3.exe : smain.obj scale3.obj
	wlink n scale3 f smain f scale3

smain.obj : smain.c

scale3.obj : scale3.c


