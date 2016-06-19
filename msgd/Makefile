CC=gcc
CFLAGS=-I../libmsg
LDFLAGS=-L../libmsg -lmsg
SOURCES=log.c mem.c node.c peer.c update.c main.c


msgd:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o msgd


all: msgd


clean:
	rm msgd
