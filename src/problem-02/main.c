/**/

#include <stdio.h>

#include "mpi.h"

#define EPSILON 0.0000000000000001
//define TAGS

/*
#define B_SIZE  200

typedef struct Buffer {
    int pieces; //current number of elements
    int size; //total number of elements
    int b[B_SIZE];
} Buffer;
*/

void master();
void calculate();

int main(int argc, char** argv) {
    int rank;
    int no_processes;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &no_processes);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) { /* Master node */
        master();
    } else { /* Calculating nodes */
        calculate()
    }

    MPI_Finalize();

    return 0;
}

void master() {
    MPI_Status status;

    //initialize buffer

    for(;;) {
        //receive any tag
        //if tag is get_interval, send next interval (protect buffer?)
        //if tag is partial_result, sum to partial result variable and sum the intervals.
        //if all intervals were calculated,
    }
}

void calculate() {
    MPI_Status status;

    for(;;) {

    }
}
