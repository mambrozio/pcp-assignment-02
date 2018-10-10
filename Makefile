#
# Makefile
#

CC := mpicc

CFLAGS := -Wall

main: objs
	@- $(CC) $(CFLAGS) obj/function.o obj/main.o -o bin/main

objs:	
	@- $(CC) $(CFLAGS) -c src/function.c -o obj/function.o
	@- $(CC) $(CFLAGS) -c src/main.c -o obj/main.o

run: clean main
	@- bin/main

clean:
	@- rm -f obj/*.o
	@- rm -f bin/*
