CC=gcc
CFLAGS=-shared -fPIC
LDFLAGS=-lrt
SOURCES=


libutil:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o libutil.so


all: libutil


clean:
	rm *.so
