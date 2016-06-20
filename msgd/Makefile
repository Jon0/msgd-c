
# directory of main library
MSGDIR=libmsg

CC=gcc
CFLAGS=-I../$(MSGDIR)
LDFLAGS=-L../$(MSGDIR) -lmsg -Wl,-rpath=$(MSGDIR)
SOURCES=log.c mem.c node.c peer.c update.c main.c


msgd:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o msgd


all: msgd


clean:
	rm msgd
