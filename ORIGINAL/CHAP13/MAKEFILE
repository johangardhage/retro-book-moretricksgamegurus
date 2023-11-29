#
#  Automatically check for file dependencies
#
.AUTODEPEND

#
#  Define a memory model - small by default
#
!if !$d(MODEL)
MODEL=s
!endif

#
#  Set compiler flags
#
!if $d(DEBUG)
CFLAGS = -O2 -3 -DDEBUG
!else
CFLAGS = -3
!endif

CC = bcc
OBJS = main.obj foo1.obj foo2.obj

#
# Make the debug version by default
#
debug:
	echo Making debug version...
	make -DDEBUG myappd

#
# Make the production version
#
nodebug:
	echo Making production version...
	make myapp

#
# Make em all
#
all:
    make clean
    touch *.c
    make debug
    touch *.c
    make nodebug

#
# Link the release version
#
myapp:      $(OBJS)
	$(CC) -m$(MODEL) $(CFLAGS) -e$@ $(OBJS)

#
# Link the debug version
#
myappd:     $(OBJS)
	$(CC) -m$(MODEL) $(CFLAGS) -e$@ $(OBJS)


#
# Clean up uneeded files
#
clean:
	del *.obj
	del *.bak
