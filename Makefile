all: STDM

STDM: STDM.o
	gcc -o STDM -g STDM.o -lm

STDM.o: STDM.c STDM.h
	gcc -g -c -Wall STDM.c

clean:
	rm -f *.o *.exe *.gch STDM
