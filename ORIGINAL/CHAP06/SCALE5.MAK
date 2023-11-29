.c.obj:
       wcc386 -w4 -4r -mf -oneatx $[*

.asm.obj:
        tasm $[* -mx -m2 -q

scale5.exe : smain.obj scale5.obj
	wlink n scale5 f smain f scale5

smain.obj : smain.c

scale5.obj : scale5.asm

