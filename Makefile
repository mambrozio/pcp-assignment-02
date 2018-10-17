#
# Makefile
#

CC := mpicc

CFLAGS := -Wall -lm

main:
	@- echo "usage:\n\tmake p1|p2"

p1:
	@- $(CC) $(CFLAGS) -c src/area.c -o obj/area.o
	@- $(CC) $(CFLAGS) -c src/problem-01/main.c -o obj/main1.o
	@- $(CC) $(CFLAGS) obj/area.o obj/main1.o -o bin/main1

p2:
	@- $(CC) $(CFLAGS) -c src/area.c -o obj/area.o
	@- $(CC) $(CFLAGS) -c src/problem-02/list.c -o obj/list.o
	@- $(CC) $(CFLAGS) -c src/problem-02/main.c -o obj/main2.o
	@- $(CC) $(CFLAGS) obj/list.o obj/area.o obj/main2.o -o bin/main2
	
run1: clean p1
	@- mpirun -np 2 bin/main1 -3 2

run2: clean p2
	@- mpirun --oversubscribe -np 4 bin/main2 -3 2

clean:
	@- rm -f obj/*.o
	@- rm -f bin/*
