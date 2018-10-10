/**/

#include <stdio.h>
#include <math.h>
#include <time.h>

#include "mpi.h"

#include "function.h"
#include "timer.h"

#define EPSILON 0.0000000000000001

int rank;
int no_processes;
double result = 0.0;
double start, end;

void print();

int main(int argc, char** argv) {

    GET_TIME(start);
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &no_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Everyone executes the same function */
    double a = 0.0;
    double b = 10.0;
    double size = (b - a) / no_processes;
    double local_result = area(exp, rank * size, (rank + 1.0) * size, EPSILON);
    GET_TIME(end);
    MPI_Reduce(&local_result, &result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Finalize();

    if (rank == 0) {
        printf("%.25f\n", result);

    }
    printf("Elapsed time %d: %f\n", rank, end - start);

    return 0;
}

void print() {
    printf("Hello, World, I'm rank #%d of %d\n", rank, no_processes);
}
