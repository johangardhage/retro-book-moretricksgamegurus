# ллллллллллллллллллллллллллллллллл
# лллллл   HEIGHT MAKEFILE   лллллл
# ллллллллллллллллллллллллллллллллл

.c.obj: .AUTODEPEND
#        wcc386 -w4 -mf -od -d2 $[*
       wcc386 -w4 -5r -mf -oneatx $[*

.asm.obj: .AUTODEPEND
#        tasm $[* -mx -m2 -zi
        tasm $[* -mx -m2 -q

dummy: height1.exe height2.exe

height1.exe : height1.obj utils.lib height1.lnk
	wlink @$^*

height1.lnk : makefile
	%create $^@
	%append $^@ NAME $^&
        %append $^@ DEBUG ALL
        %append $^@ FILE height1.obj
        %append $^@ LIB utils
        %append $^@ OPTION
	%append $^@   VERBOSE,
	%append $^@   STACK=12000,
	%append $^@   OFFSET=64K,
	%append $^@   DOSSEG

height2.exe : height2.obj height2a.obj utils.lib height2.lnk
	wlink @$^*

height2.lnk : makefile
	%create $^@
	%append $^@ NAME $^&
        %append $^@ DEBUG ALL
        %append $^@ FILE height2.obj
        %append $^@ FILE height2a.obj
        %append $^@ LIB utils
        %append $^@ OPTION
	%append $^@   VERBOSE,
	%append $^@   STACK=12000,
	%append $^@   OFFSET=64K,
	%append $^@   DOSSEG


