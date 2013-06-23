include config.mk

SRC = joyexec.c
OBJ = ${SRC:.c=.o}

all: joyexec

joyexec: ${OBJ} config.mk
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

.c.o: common.h
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

clean:
	rm joyexec *.o

install: joyexec
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f joyexec ${DESTDIR}${PREFIX}/bin
	@echo installing manual page to ${DESTDIR}${MANPREFIX}/man1
	@mkdir -p ${DESTDIR}${MANPREFIX}/man1
	@sed "s/VERSION/${VERSION}/g" < joyexec.1 > ${DESTDIR}${MANPREFIX}/man1/joyexec.1
	@chmod 644 ${DESTDIR}${MANPREFIX}/man1/joyexec.1
	@mkdir -p ${DESTDIR}${ETCPREFIX}/init.d
	@sed "s#PREFIX#${PREFIX}#g" < joyexec.init > ${DESTDIR}${ETCPREFIX}/init.d/joyexec
	@chmod 755 ${DESTDIR}${PREFIX}/bin/joyexec ${DESTDIR}${ETCPREFIX}/init.d/joyexec
	@cp -f joyexec.conf ${DESTDIR}${ETCPREFIX}/joyexec.conf

uninstall:
	rm ${DESTDIR}${PREFIX}/bin/joyexec \
		${DESTDIR}${MANPREFIX}/man1/joyexec.1 \
		${DESTDIR}${INITPREFIX}/init.d/joyexec \
		${DESTDIR}${ETCPREFIX}/joyexec.conf

.PHONY: install uninstall clean
