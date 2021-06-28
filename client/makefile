COMPILE_OPTION=`pkg-config --cflags --libs gtk+-3.0 gthread-2.0`
CFILES:=$(wildcard *.c)
EXES:=$(patsubst %.c, %, $(CFILES))
gtk2:
	gcc main_gtk.c -g -o client ${COMPILE_OPTION} -w -I ../server
all:
	echo $(EXES)
clean:
	rm -f $(EXES)
.PHONY:clean all
