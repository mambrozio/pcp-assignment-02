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

p21:
	@- $(CC) $(CFLAGS) -c src/area.c -o obj/area.o
	@- $(CC) $(CFLAGS) -c src/problem-02/list.c -o obj/list.o
	@- $(CC) $(CFLAGS) -c src/problem-02/version-01/main.c -o obj/main21.o
	@- $(CC) $(CFLAGS) obj/list.o obj/area.o obj/main21.o -o bin/main21

p22:
	@- $(CC) $(CFLAGS) -c src/area.c -o obj/area.o
	@- $(CC) $(CFLAGS) -c src/problem-02/list.c -o obj/list.o
	@- $(CC) $(CFLAGS) -c src/problem-02/version-02/main.c -o obj/main22.o
	@- $(CC) $(CFLAGS) obj/list.o obj/area.o obj/main22.o -o bin/main22

run1: clean p1
	@- mpirun -np 2 bin/main1 0 1

run21: clean p21
	@- mpirun --oversubscribe -np 2 bin/main21 0 1

run22: clean p22
	@- mpirun --oversubscribe -np 2 bin/main22 0 1

clean:
	@- rm -f obj/*.o
	@- rm -f bin/*
