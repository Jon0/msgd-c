
# directory of main library
MSGDIR=libmsg
MSGREL=../$(MSGDIR)

CC=gcc
CFLAGS=-I$(MSGREL)
LDFLAGS=-L$(MSGREL) -lmsg -Wl,-rpath=$(MSGDIR)
SOURCES=main.c


msgd:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o msgd


all: msgd


clean:
	rm msgd
