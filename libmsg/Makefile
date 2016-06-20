CC=gcc
CFLAGS=-shared -fPIC
LDFLAGS=-lrt
SOURCES=msgd.c queue.c route.c share.c


libmsg:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o libmsg.so


all: msg


clean:
	rm libmsg.so
