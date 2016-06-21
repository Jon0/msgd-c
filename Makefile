MAKE=make

all:
	$(MAKE) -C libutil
	$(MAKE) -C libmsg
	$(MAKE) -C modules
	$(MAKE) -C msgd
	$(MAKE) -C tests

clean:
	$(MAKE) clean -C libutil
	$(MAKE) clean -C libmsg
	$(MAKE) clean -C modules
	$(MAKE) clean -C msgd
	$(MAKE) clean -C tests

pull:
	git pull


test:
	./msgd/msgd modules/init/libmsgd-init.so
