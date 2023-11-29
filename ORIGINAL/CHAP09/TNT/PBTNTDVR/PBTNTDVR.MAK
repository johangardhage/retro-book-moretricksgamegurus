# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "pbtntdvr.mak" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : .\WinRel\pbtntdvr.exe .\WinRel\pbtntdvr.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /ML /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /ML /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FR /c
CPP_PROJ=/nologo /ML /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"pbtntdvr.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"pbtntdvr.bsc" 
BSC32_SBRS= \
	.\WinRel\GKS.SBR \
	.\WinRel\pbtntdvr.sbr \
	.\WinRel\IFF.SBR \
	.\WinRel\PIXBUF.SBR \
	.\WinRel\VTOOLS.SBR

.\WinRel\pbtntdvr.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:console /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dosx32.lib pharx32.lib /NOLOGO /SUBSYSTEM:console /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib dosx32.lib pharx32.lib /NOLOGO /SUBSYSTEM:console /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"pbtntdvr.pdb" /MACHINE:I386 /OUT:$(OUTDIR)/"pbtntdvr.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinRel\GKS.OBJ \
	.\WinRel\pbtntdvr.obj \
	.\WinRel\IFF.OBJ \
	.\WinRel\PIXBUF.OBJ \
	.\WinRel\VTOOLS.OBJ

.\WinRel\pbtntdvr.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : .\WinDebug\pbtntdvr.exe .\WinDebug\pbtntdvr.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /ML /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /ML /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FR /c
CPP_PROJ=/nologo /ML /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"pbtntdvr.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"pbtntdvr.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"pbtntdvr.bsc" 
BSC32_SBRS= \
	.\WinDebug\GKS.SBR \
	.\WinDebug\pbtntdvr.sbr \
	.\WinDebug\IFF.SBR \
	.\WinDebug\PIXBUF.SBR \
	.\WinDebug\VTOOLS.SBR

.\WinDebug\pbtntdvr.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /NOLOGO /SUBSYSTEM:console /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"pbtntdvr.pdb" /DEBUG /MACHINE:I386\
 /OUT:$(OUTDIR)/"pbtntdvr.exe" 
DEF_FILE=
LINK32_OBJS= \
	.\WinDebug\GKS.OBJ \
	.\WinDebug\pbtntdvr.obj \
	.\WinDebug\IFF.OBJ \
	.\WinDebug\PIXBUF.OBJ \
	.\WinDebug\VTOOLS.OBJ

.\WinDebug\pbtntdvr.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=\RELEASE\SOURCE\TNT\GKS.CPP
DEP_GKS_C=\
	\RELEASE\SOURCE\TNT\gks.hpp

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\GKS.OBJ :  $(SOURCE)  $(DEP_GKS_C) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\GKS.OBJ :  $(SOURCE)  $(DEP_GKS_C) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\RELEASE\SOURCE\TNT\pbtntdvr.cpp
DEP_PBTNT=\
	\tnt\include\pharlap.h\
	\RELEASE\SOURCE\TNT\gks.hpp\
	\RELEASE\SOURCE\TNT\vtools.hpp\
	\RELEASE\SOURCE\TNT\iff.hpp\
	\RELEASE\SOURCE\TNT\pixbuf.hpp\
	\tnt\include\pltypes.h

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\pbtntdvr.obj :  $(SOURCE)  $(DEP_PBTNT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\pbtntdvr.obj :  $(SOURCE)  $(DEP_PBTNT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\RELEASE\SOURCE\TNT\IFF.CPP
DEP_IFF_C=\
	\RELEASE\SOURCE\TNT\iff.hpp\
	\RELEASE\SOURCE\TNT\pixbuf.hpp\
	\RELEASE\SOURCE\TNT\vtools.hpp\
	\RELEASE\SOURCE\TNT\gks.hpp

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\IFF.OBJ :  $(SOURCE)  $(DEP_IFF_C) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\IFF.OBJ :  $(SOURCE)  $(DEP_IFF_C) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\RELEASE\SOURCE\TNT\PIXBUF.CPP
DEP_PIXBU=\
	\RELEASE\SOURCE\TNT\pixbuf.hpp\
	\RELEASE\SOURCE\TNT\gks.hpp

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\PIXBUF.OBJ :  $(SOURCE)  $(DEP_PIXBU) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\PIXBUF.OBJ :  $(SOURCE)  $(DEP_PIXBU) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\RELEASE\SOURCE\TNT\VTOOLS.CPP
DEP_VTOOL=\
	\tnt\include\pharlap.h\
	\tnt\include\pldos32.h\
	\RELEASE\SOURCE\TNT\vtools.hpp\
	\RELEASE\SOURCE\TNT\pixbuf.hpp\
	\tnt\include\pltypes.h\
	\RELEASE\SOURCE\TNT\gks.hpp

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\VTOOLS.OBJ :  $(SOURCE)  $(DEP_VTOOL) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\VTOOLS.OBJ :  $(SOURCE)  $(DEP_VTOOL) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
