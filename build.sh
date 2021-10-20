#!/bin/sh
if [ -z "$1" ] ; then
	echo error, pass filename of file containing mymemcpy
	exit 1
fi

if test -z "$CC" -o -z "$OPTS" ; then
	echo "warning: CC or OPTS not set, which is probably not what you want" >&2
fi

[ -z "$CC" ] && CC=gcc
$CC -c dummyfuncs.c -o dummyfuncs.o
$CC -D_GNU_SOURCE -D_BSD_SOURCE -O0 -g3 -DFILENAME=\"$1\" -c memcpy_test.c -o memcpy_test.o
$CC $OPTS -c "$1" -o mymemcpy.o
$CC dummyfuncs.o memcpy_test.o mymemcpy.o -o test
