CC=gcc
CFLAGS=-shared -fPIC
LDFLAGS=
SOURCES=msgd.c


libmsg:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o libmsg.so


all: msg


clean:
	rm libmsg.so
