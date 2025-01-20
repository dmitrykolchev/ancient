.AUTODEPEND

#		*Translator Definitions*
CC = bcc +PM.CFG
TASM = TASM
TLIB = tlib
TLINK = tlink
LIBPATH = D:\BORLANDC\LIB
INCLUDEPATH = D:\BORLANDC\INCLUDE


#		*Implicit Rules*
.c.obj:
  $(CC) -c {$< }

.cpp.obj:
  $(CC) -c {$< }

#		*List Macros*


EXE_dependencies =  \
 pmmain.obj \
 pminit.obj \
 pmtable.obj \
 trace.obj \
 inter.obj \
 deasm.obj \
 cmd0f.obj \
 keyboard.obj \
 setint.obj

#		*Explicit Rules*
pm.exe: pm.cfg $(EXE_dependencies)
  $(TLINK) /v/3/c/P-/L$(LIBPATH) @&&|
c0s.obj+
pmmain.obj+
pminit.obj+
pmtable.obj+
trace.obj+
inter.obj+
deasm.obj+
cmd0f.obj+
keyboard.obj+
setint.obj
pm
		# no map file
cs.lib
|


#		*Individual File Dependencies*
pmmain.obj: pm.cfg pmmain.c 

pminit.obj: pm.cfg pminit.c 

pmtable.obj: pm.cfg pmtable.asm 
	$(TASM) /MX /ZI /O PMTABLE.ASM,PMTABLE.OBJ

trace.obj: pm.cfg trace.asm 
	$(TASM) /MX /ZI /O TRACE.ASM,TRACE.OBJ

inter.obj: pm.cfg inter.cpp 

deasm.obj: pm.cfg deasm.cpp 

cmd0f.obj: pm.cfg cmd0f.cpp 

keyboard.obj: pm.cfg keyboard.c 

setint.obj: pm.cfg setint.asm 
	$(TASM) /MX /ZI /O SETINT.ASM,SETINT.OBJ

#		*Compiler Configuration File*
pm.cfg: pm.mak
  copy &&|
-3
-f-
-ff-
-v
-y
-G
-O
-Og
-Oe
-Om
-Ov
-Ol
-Ob
-Op
-Oi
-Z
-k-
-vi-
-wpro
-weas
-wpre
-I$(INCLUDEPATH)
-L$(LIBPATH)
| pm.cfg


