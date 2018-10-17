/**/

#include <stdio.h>
#include <stddef.h> //ofsetof
#include <mpi.h>

#include "list.h"

#define EPSILON 0.0000000000000001
#define EMPTY 0

// TAGS
#define TAG_WANT_INTERVAL  3//Calculating node wants interval from master
#define TAG_PARTIAL_RESULT 5//Calculating node sends partial result to master
#define TAG_NEW_INTERVAL   7//Calculating node sends new interval to master

typedef struct Message {
    double a;
    double b;
    double res;
} Message;

typedef struct Interval {
    double a;
    double b;
} Interval;

void master();
void calculate();

/* Global variables */
int rank;
int no_processes; //need to be global so master can access to create waiting_queue
int init_interval = 0;
int end_interval = 10;

int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &no_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Create new mpi type for struct Message
    MPI_Datatype mpi_dt_message;
    MPI_Type_create_struct(3, {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE}, {1, 1, 1},
        {offsetof(Message, a), offsetof(Message, b), offsetof(Message, res)},
        &mpi_dt_message);

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
    unsigned waiting[no_processes];
    Message msg;
    List* intervals = list_new();
    Interval interval;

    for (int i = 0; i < no_processes; i++) {
        waiting[i] = EMPTY;
    }

    double size = (b - a) / no_processes;
    for (int i = 0; i < no_processes - 1; i++) {
        interval.a = (double)i * size;
        interval.b = ((double)i + 1) * size;
        list_append(intervals, interval);
    }

    for(;;) {
        // Receive any tag
        MPI_Recv(&msg, mpi_dt_message, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        switch (status.MPI_TAG) {
            //if tag is get_interval, send next interval (protect buffer?)
            case TAG_WANT_INTERVAL:
                break;
            //if tag is partial_result, sum to partial result variable and sum the intervals
            case TAG_PARTIAL_RESULT:
                break;
            //if tag is new_interval, insert new_interval in intervals list
            case TAG_NEW_INTERVAL:
                break;
            default:
                fprintf(stderr, "Node %d sent unknown tag %d\n", status.MPI_SOURCE, status.MPI_TAG);
                break;
        }

        //if all intervals were calculated, send message to all nodes to terminate
    }

    return;
}

void calculate() {
    MPI_Status status;

    for(;;) {
        //check if asking for interval is needed
        //calculate interval
        //if split is needed, make new interval, send one to master, update current interval
        //else, send result to master and clean current interval
    }

    return;
}
