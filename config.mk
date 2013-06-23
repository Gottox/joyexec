# version
VERSION = 0.1

# Customize below to fit your system

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man
ETCPREFIX = /etc

CONFIGINC = /usr/include
CONFIGLIB = /usr/lib


# includes and libs
INCS = -I${CONFIGINC}
LIBS = -L${CONFIGLIB} -lconfig 

# flags
CPPFLAGS = -D_BSD_SOURCE -D_POSIX_C_SOURCE=2 -DVERSION=\"${VERSION}\" -DETCPREFIX=\"${ETCPREFIX}\"
CFLAGS   = -std=c99 -pedantic -Wall -Wno-deprecated-declarations ${INCS} ${CPPFLAGS} -g
LDFLAGS  = -s ${LIBS}

# compiler and linker
CC = cc
