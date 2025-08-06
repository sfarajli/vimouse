.POSIX:

CC       = tcc
LDFLAGS  = -lX11 -lXtst
CFLAGS   = -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Os -D_POSIX_C_SOURCE=199309L

PREFIX   ?= /usr/local

vimouse: vimouse.c
	${CC} -o $@ ${CFLAGS} ${LDFLAGS} $^

install: vimouse
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f $< ${DESTDIR}${PREFIX}/bin

clean:
	rm -f vimouse

.PHONY: install clean
