CC=gcc
CFLAGS=-I../libmsg
LDFLAGS=-L../libmsg -lmsg
SOURCES=test.c


test:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o test


all: test


clean:
	rm test
