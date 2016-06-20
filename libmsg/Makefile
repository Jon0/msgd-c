CC=gcc
CFLAGS=-shared -fPIC
LDFLAGS=-lrt -ldl
SOURCES=mem.c module.c msgd.c node.c queue.c route.c share.c update.c


libmsg:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o libmsg.so


all: libmsg


clean:
	rm libmsg.so
