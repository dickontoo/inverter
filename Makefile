CC=gcc
LD=gcc

CFLAGS=-Wall -O3

LIBS=
OFILES=inverter.o

.PHONY: all clean install

all: inverter

.c.o: inverter.h
	$(CC) $(CFLAGS) -c $<

inverter: $(OFILES) main.o
	$(CC) $(LDFLAGS) $(LIBS) -o inverter main.o $(OFILES)

clean:
	rm -f *.o inverter
