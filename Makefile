.POSIX:

PREFIX ?= /usr/local

vimouse: vimouse.c
	gcc -o $@ $< -lX11 -lXtst

install: vimouse
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f $< ${DESTDIR}${PREFIX}/bin

clean:
	rm -f vimouse

.PHONY: install clean
