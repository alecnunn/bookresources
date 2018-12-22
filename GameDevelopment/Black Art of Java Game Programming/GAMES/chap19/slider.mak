ORIGIN		= Symantec Café
ORIGIN_VER	= Version 7.22
VERSION		= DEBUG

!IFDEF SUB_DEBUG
DEBUG		= $(SUB_DEBUG)
NDEBUG		= !$(SUB_DEBUG)
!ELSE
DEBUG		= 1
NDEBUG		= 0
!ENDIF

PROJ		= Slider
APPTYPE		= JAVA APPLET
PROJTYPE	= class

JAVAC		= sj
MAKE		= SMAKE
LNK		= LINK


HEADERS		= 
!IF $(DEBUG)
OUTPUTDIR	= .
CREATEOUTPUTDIR	=
TARGETDIR	= .
CREATETARGETDIR	=

CFLAGS		=  -g 
DEFINES		= 
!ELSE
OUTPUTDIR	= .
CREATEOUTPUTDIR	=
TARGETDIR	= .
CREATETARGETDIR	=

CFLAGS		=  -O 
DEFINES		= 
!ENDIF

MFLAGS		= MASTERPROJ=$(PROJ) 
DEBUGGERFLAGS	=  
PAR		= PROJS BATS OBJS

INCLUDES	= 



OBJS		= Slider.class 

BATS		= 

.SUFFIXES: .JAVA .CLASS 

.JAVA.CLASS:
	$(JAVAC) $(CFLAGS) $(INCLUDES) $*.java



all:	noteout createdir $(PRECOMPILE) $(OBJS) $(POSTCOMPILE) _done


all2:	createdir $(PRECOMPILE) $(OBJS) $(POSTCOMPILE) _done

noteout:
	REM Output to $(OUTPUTDIR)

createdir:
	$(CREATEOUTPUTDIR)
	$(CREATETARGETDIR)
	
_done:
		REM  Project is up to date

buildall:	clean	all


clean:
		-del $(TARGETDIR)\$$SCW$$.$(PROJTYPE)
		-del $(TARGETDIR)\$(PROJ).CLE
		-del Slider.dpd
		-del $(OBJS)



!IF EXIST (Slider.dpd)
!INCLUDE Slider.dpd
!ENDIF

