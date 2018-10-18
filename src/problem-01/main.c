#include <assert.h>
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../area.h"
#include "../timer.h"

#include "../weird.c"

#define EPSILON 0.0000000000000001

int rank;
int no_processes;
double result = 0.0;
double start, end;

void print();

int main(int argc, char** argv) {
    GET_TIME(start);

    // arguments
    MPI_Init(&argc, &argv);
    assert(argc == 3);
    double lower_bound = strtod(argv[1], NULL);
    double upper_bound = strtod(argv[2], NULL);

    MPI_Comm_size(MPI_COMM_WORLD, &no_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // everyone executes the same function
    Function f = weird1;
    double size = (upper_bound - lower_bound) / no_processes;
    double lb /* local lower bound */ = lower_bound + rank * size;
    double ub /* local upper bound */ = lower_bound + (rank + 1.0) * size;
    double local = calculate_area_recursively(f, lb, ub, f(lb), f(ub), EPSILON);

    GET_TIME(end);
    MPI_Reduce(&local, &result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

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
