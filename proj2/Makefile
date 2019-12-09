CFLAGS = -g -std=c11 -O2 -Wall -pedantic `pkg-config --cflags gtk+-2.0`
LDLIBS = `pkg-config --libs gtk+-2.0`

all : calc 

calc : calc.o
	$(CC) -o calc calc.o $(LDLIBS) -lm

clean :
	rm -f calc.o calc
