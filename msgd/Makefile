CC=gcc
CFLAGS=-I../libmsg
LDFLAGS=-L../libmsg -lmsg
SOURCES=main.c


msgd:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o msgd


all: msgd


clean:
	rm msgd
