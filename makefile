#Mouse Injector Makefile
#Use with TDM-GCC 4.9.2-tdm-3 or MinGW
#For portable MinGW download Orwell Dev-C++
#mingw32-make.exe -f makefile to compile

#Compiler directories
MINGWDIR = C:/MinGW64/bin/
CC = $(MINGWDIR)gcc
WINDRES = $(MINGWDIR)windres

#Source directories
SRCDIR = ./
MANYMOUSEDIR = $(SRCDIR)manymouse/
GAMESDIR = $(SRCDIR)games/
OBJDIR = $(SRCDIR)obj/
UIDIR = $(SRCDIR)ui/
DLLNAME = $(SRCDIR)Mouse_Injector.dll

#Compiler flags
#USE_DISCORD_PRESENCE = uncomment line to add discord rich presence (requires clean and rebuild)
#SPEEDRUN_BUILD = uncomment line to build speedrun version (requires clean and rebuild)
CFLAGS = -ansi -O2 -m32 -std=c11 -Wall
ifdef USE_DISCORD_PRESENCE
	CFLAGS += -DADD_DISCORD_PRESENCE
endif
ifdef SPEEDRUN_BUILD
	CFLAGS += -DSPEEDRUN_BUILD
	DLLNAME = $(SRCDIR)Mouse_Injector_Speedrun.dll
endif
WARNINGS = -Wextra -pedantic -Wunreachable-code -Wno-parentheses -Wno-strict-aliasing
RESFLAGS = -F pe-i386 --input-format=rc -O coff

#Linker flags
OBJS = $(OBJDIR)maindll.o $(OBJDIR)device.o $(OBJDIR)discord.o $(OBJDIR)manymouse.o $(OBJDIR)windows_wminput.o $(OBJDIR)game.o $(OBJDIR)goldeneye.o $(OBJDIR)perfectdark.o $(OBJDIR)ui.res
LIBS = -static-libgcc
ifdef USE_DISCORD_PRESENCE
	LIBS += -L./discord -ldiscord-rpc
endif
LFLAGS = -shared $(OBJS) -o $(DLLNAME) $(LIBS) -m32 -s -Wl,--add-stdcall-alias

#Main recipes
mouseinjector: $(OBJS)
	$(CC) $(LFLAGS)

all: clean mouseinjector

#Individual recipes
$(OBJDIR)maindll.o: $(SRCDIR)maindll.c $(SRCDIR)global.h $(SRCDIR)maindll.h $(SRCDIR)device.h $(SRCDIR)discord.h $(UIDIR)ui.rc $(UIDIR)resource.h $(SRCDIR)vkey.h
	$(CC) -c $(SRCDIR)maindll.c -o $(OBJDIR)maindll.o $(CFLAGS) $(WARNINGS) -Wno-unused-parameter

$(OBJDIR)device.o: $(SRCDIR)device.c $(SRCDIR)global.h $(SRCDIR)device.h $(SRCDIR)maindll.h $(MANYMOUSEDIR)manymouse.h $(GAMESDIR)game.h
	$(CC) -c $(SRCDIR)device.c -o $(OBJDIR)device.o $(CFLAGS) $(WARNINGS)

$(OBJDIR)discord.o: $(SRCDIR)discord.c $(SRCDIR)global.h $(SRCDIR)discord.h $(SRCDIR)maindll.h $(GAMESDIR)game.h $(GAMESDIR)memory.h
	$(CC) -c $(SRCDIR)discord.c -o $(OBJDIR)discord.o $(CFLAGS) $(WARNINGS)

$(OBJDIR)manymouse.o: $(MANYMOUSEDIR)manymouse.c $(MANYMOUSEDIR)manymouse.h
	$(CC) -c $(MANYMOUSEDIR)manymouse.c -o $(OBJDIR)manymouse.o $(CFLAGS) $(WARNINGS)

$(OBJDIR)windows_wminput.o: $(MANYMOUSEDIR)windows_wminput.c $(MANYMOUSEDIR)manymouse.h
	$(CC) -c $(MANYMOUSEDIR)windows_wminput.c -o $(OBJDIR)windows_wminput.o $(CFLAGS)

$(OBJDIR)game.o: $(GAMESDIR)game.c $(GAMESDIR)game.h
	$(CC) -c $(GAMESDIR)game.c -o $(OBJDIR)game.o $(CFLAGS) $(WARNINGS)

$(OBJDIR)goldeneye.o: $(GAMESDIR)goldeneye.c $(SRCDIR)global.h $(SRCDIR)device.h $(SRCDIR)maindll.h $(GAMESDIR)game.h $(GAMESDIR)memory.h
	$(CC) -c $(GAMESDIR)goldeneye.c -o $(OBJDIR)goldeneye.o $(CFLAGS) $(WARNINGS)

$(OBJDIR)perfectdark.o: $(GAMESDIR)perfectdark.c $(SRCDIR)global.h $(SRCDIR)device.h $(SRCDIR)maindll.h $(GAMESDIR)game.h $(GAMESDIR)memory.h
	$(CC) -c $(GAMESDIR)perfectdark.c -o $(OBJDIR)perfectdark.o $(CFLAGS) $(WARNINGS)

$(OBJDIR)ui.res: $(UIDIR)ui.rc $(UIDIR)resource.h
	$(WINDRES) -i $(UIDIR)ui.rc -o $(OBJDIR)ui.res $(RESFLAGS)

clean:
	rm -f $(SRCDIR)*.dll $(OBJDIR)*.o $(OBJDIR)*.res