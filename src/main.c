/**/

#include <stdio.h>

#include "mpi.h"

int rank;
int no_processes;

void print();

int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &no_processes);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Everyone executes the same function */
    print();

    MPI_Finalize();

    return 0;
}

void print() {
    printf("Hello, World, I'm rank #%d of %d\n", rank, no_processes);
}
