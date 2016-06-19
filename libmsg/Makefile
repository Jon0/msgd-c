CC=gcc
CFLAGS=-shared -fPIC
LDFLAGS=
SOURCES=log.c mem.c node.c peer.c update.c


libmsg:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o libmsg.so


all: msg


clean:
	rm libmsg.so
