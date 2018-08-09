#
# Copyright (C) 1996 Paul Henson. Please see file COPYRIGHT for details
#

# Set to the C compiler you wish to you. Must be ANSI.
CC = gcc

# Set to the linker you wish to use, generally the same as CC.
LD = gcc

# Set to the location of your ncurses include files. 
NCURSES_INCLUDE = -I/usr/local/include -I/usr/local/include/ncurses

# Set to the location of your ncurses library
NCURSES_LIB = -L/usr/local/lib -lncurses

# Add any include paths you might need here
INCLUDES = $(NCURSES_INCLUDE)

# Add any extra libraries you need here. For example, Solaris requires
# the addition of "-lsocket -lnsl" for network support
LDLIBS = $(NCURSES_LIB)

# Uncomment this for debugging
#DEFINES = -g -DDEBUG
DEFINES = -O

# Set to installation directory for binary
INSTALLDIR = /usr/local/bin

# Set to installation directory for man page
MANDIR = /usr/local/man/man1

# Set to extension for man page
MANEXT = 1

# ---------------------------------------------------------------------------
# Nothing user configurable past here

OBJS = dohthello.o board_logic.o generic_interface.o curses_interface.o \
       xmotif_interface.o debug.o network.o

all: dohthello

dohthello: $(OBJS)
	$(LD) -o dohthello $(OBJS) $(LDLIBS)

dohthello.o: dohthello.c dohthello.h board_logic.h curses_interface.h network.h
	$(CC) $(DEFINES) $(INCLUDES) -c dohthello.c

board_logic.o: board_logic.c board_logic.h
	$(CC) $(DEFINES) $(INCLUDES) -c board_logic.c

generic_interface.o: generic_interface.c generic_interface.h curses_interface.h xmotif_interface.h dohthello.h
	$(CC) $(DEFINES) $(INCLUDES) -c generic_interface.c

curses_interface.o: curses_interface.c curses_interface.h dohthello.h
	$(CC) $(DEFINES) $(INCLUDES) -c curses_interface.c

xmotif_interface.o: xmotif_interface.c xmotif_interface.h dohthello.h
	$(CC) $(DEFINES) $(INCLUDES) -c xmotif_interface.c

network.o: network.c network.h
	$(CC) $(DEFINES) $(INCLUDES) -c network.c

debug.o: debug.c debug.h
	$(CC) $(DEFINES) $(INCLUDES) -c debug.c

clean:
	rm -f dohthello *.o

install: all
	cp dohthello $(INSTALLDIR)/dohthello
	chmod a+rx $(INSTALLDIR)/dohthello
	cp dohthello.man $(MANDIR)/dohthello.$(MANEXT)
	chmod a+r $(MANDIR)/dohthello.$(MANEXT)
