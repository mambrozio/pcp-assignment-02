#
# Makefile
#

CC := mpicc

CFLAGS := -Wall -lm

main: objs
	@- $(CC) $(CFLAGS) obj/area.o obj/main.o -o bin/main

objs:
	@- $(CC) $(CFLAGS) -c src/area.c -o obj/area.o
	@- $(CC) $(CFLAGS) -c src/main.c -o obj/main.o

run: clean main
	@- mpirun -np 2 bin/main -3 2

clean:
	@- rm -f obj/*.o
	@- rm -f bin/*
