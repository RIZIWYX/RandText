.PHONY: all clean dist
all:
	$(MAKE) -C randtext
clean:
	$(MAKE) -C randtext clean
dist: clean
	tar -hzcf ../randtext.tar.gz README.md tests/* hashtable/* holdall/* malloccp/* randtext/* doc/* makefile
