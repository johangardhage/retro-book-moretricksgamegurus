.c.obj:
       wcc386 -w4 -4r -mf -oneatx $[*

.asm.obj:
        tasm $[* -mx -m2 -q

scale6.exe : smain.obj scale6.obj
	wlink n scale6 f smain f scale6

smain.obj : smain.c

scale6.obj : scale6.asm

