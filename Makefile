CC=gcc
LD=gcc

CFLAGS=-Wall -O3

LIBS=
OFILES=inverter.o main.o

.PHONY: all clean install

all: inverter

.c.o: inverter.h
	$(CC) $(CFLAGS) -c $<

inverter: $(OFILES)
	$(CC) $(LDFLAGS) $(LIBS) -o inverter $(OFILES)

clean:
	rm -f *.o inverter
