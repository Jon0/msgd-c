MODNAME=init

# directory of main library
MSGDIR=libmsg
MSGREL=../../$(MSGDIR)

CC=gcc
CFLAGS=-I$(MSGREL) -shared -fPIC
LDFLAGS=-L$(MSGREL) -lmsg -Wl,-rpath=$(MSGDIR)
SOURCES=init.c


$(MODNAME):
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o libmsgd-$(MODNAME).so


all: $(MODNAME)


clean:
	rm libmsgd-$(MODNAME).so
