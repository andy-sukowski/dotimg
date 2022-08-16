# See LICENSE file for copyright and license details.

include config.mk

SRC = dotimg.c png_in.c
OBJ = ${SRC:.c=.o}

all: options dotimg

options:
	@echo dotimg build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	${CC} -c ${CFLAGS} $<

dotimg: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f dotimg ${OBJ}

install: all
	mkdir -p ${PREFIX}/bin
	cp -f dotimg ${PREFIX}/bin
	chmod 755 ${PREFIX}/bin/dotimg

uninstall:
	rm -f ${PREFIX}/bin/dotimg

.PHONY: all options clean install uninstall
