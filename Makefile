CC = gcc
LIBTCODDIR = ../libtcod-1.5.0
INCDIR = $(LIBTCODDIR)/include
CFLAGS = -Wall -g -ggdb -I$(INCDIR) -I.
LDFLAGS = -L$(LIBTCODDIR) -ltcod -Wl,-rpath=lib 

SOURCES = gt.c world.c namegen.c display.c dungeon.c heightmap.c cmd.c cards.c objects.c you.c creature.c worldview.c inventory.c attack.c
HEADERS = gt.h world.h namegen.h display.h dungeon.h heightmap.h cmd.h cards.h objects.h you.h creature.h worldview.h inventory.h monslist.h
OBJS    = gt.o world.o namegen.o display.o dungeon.o heightmap.o cmd.o cards.o objects.o you.o creature.o worldview.o inventory.o attack.o

MKOBJS = objects.o makeobjdefs.o
MKSRCS = objects.c makeobjdefs.c
MKHDRS = objects.h objlist.h

gt: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

gt.: $(SOURCES) $(HEADERS)

makeobjdefs: $(MKOBJS) $(MKHDRS) $(MKSRCS) 
	$(CC) $(LDFLAGS) -o $@ $(MKOBJS) 
	./makeobjdefs

makeobjdefs.: $(MKSRCS) $(MKHDRS)

clean:
	rm *.o gt


all: makeobjdefs gt
