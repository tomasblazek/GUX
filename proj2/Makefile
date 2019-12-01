CFLAGS = -g -O2 -Wall -DGTK_DISABLE_DEPRECATED=1 -DGDK_DISABLE_DEPRECATED=1 -DG_DISABLE_DEPRECATED=1 `pkg-config --cflags gtk+-2.0`
LDLIBS = `pkg-config --libs gtk+-2.0`

all : calc 

calc : calc.o
	$(CC) -o calc calc.o $(LDLIBS)

clean :
	rm -f calc.o calc
