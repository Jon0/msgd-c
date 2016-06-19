MAKE=make

all:
	$(MAKE) -C libmsg
	$(MAKE) -C msgd
	$(MAKE) -C tests

clean:
	$(MAKE) clean -C libmsg
	$(MAKE) clean -C msgd
	$(MAKE) clean -C tests
