
# directory of main library
MSGDIR=libmsg

CC=gcc
CFLAGS=-I../$(MSGDIR)
LDFLAGS=-L../$(MSGDIR) -lmsg -Wl,-rpath=$(MSGDIR)
SOURCES=test.c


test:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o test


all: test


clean:
	rm test
