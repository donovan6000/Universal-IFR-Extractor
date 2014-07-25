# To confront my foe, an unwieldy commandline, I shall copy
# random Makefiles off the Interwebs, until one works, eschewing
# comprehension in favor of blissful ignorance. 

CC=cl
LINK=link
RC=rc.exe
LINKFLAGS= /nologo /OPT:REF /OPT:ICF /SUBSYSTEM:Windows /DYNAMICBASE /RELEASE /VERSION:0.61
LIBS=kernel32.lib user32.lib gdi32.lib shell32.lib uuid.lib comdlg32.lib
CPPFLAGS= /nologo /W3 /Ox /EHsc /J /TP /favor:blend 
CPPSOURCES = EFI.cpp UEFI.cpp main.cpp
RCFILES = "Universal IFR Extractor.rc"
RESFILES = "Universal IFR Extractor.res"
DEPS = efi.h uefi.h
OBJ = efi.obj uefi.obj main.obj
PRJNAME = Universal-IFR-Extractor-MOD.exe

all: $(PRJNAME)
	$(LINK) $(LINKFLAGS) $(OBJ) $(RESFILES) /out:$(PRJNAME) $(LIBS)

.cpp.obj:
	$(cc) $(cflags) $*.cpp
	
$(RESFILES): $(RCFILES)
	$(RC) /l 409 /v $(RCFILES)
	
$(OBJ): $(DEPS) $(CPPSOURCES)
	$(CC) $(CPPFLAGS) /c $(CPPSOURCES) 

$(PRJNAME): $(OBJ) $(RESFILES)


