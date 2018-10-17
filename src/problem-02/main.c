/**/

#include <stdio.h>
#include <stddef.h> //ofsetof
#include <mpi.h>

#include "list.h"

#define EPSILON 0.0000000000000001
#define NOT_WAITING 0
#define WAITING 1

// TAGS
#define TAG_WANT_INTERVAL    3//Calculating node wants interval from master
#define TAG_PARTIAL_RESULT   5//Calculating node sends partial result to master
#define TAG_NEW_INTERVAL     7//Calculating node sends new interval to master
#define TAG_RECEIVE_INTERVAL 9//Master sends new interval to calculating node

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
    assert(argc == 3);
    double lower_bound = strtod(argv[1], NULL);
    double upper_bound = strtod(argv[2], NULL);

    MPI_Comm_size(MPI_COMM_WORLD, &no_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Create new mpi type for struct Message
    MPI_Datatype mpi_dt_message;
    MPI_Type_create_struct(3, {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE}, {1, 1, 1},
        {offsetof(Message, a), offsetof(Message, b), offsetof(Message, res)},
        &mpi_dt_message);

    if (rank == 0) { /* Master node */
        master(lower_bound, upper_bound);
    } else { /* Calculating nodes */
        calculate();
    }

    MPI_Finalize();

    return 0;
}

void master(double a, double b) {
    MPI_Status status;
    unsigned waiting[no_processes];
    unsigned splits = no_processes - 1;
    Message msg;
    List* intervals = list_new();
    Interval* interval;
    double result = 0.0;

    for (int i = 0; i < no_processes; i++) {
        waiting[i] = NOT_WAITING;
    }

    double size = (b - a) / no_processes;
    for (int i = 0; i < no_processes - 1; i++) {
        MALLOC(interval, Interval);
        interval->a = (double)i * size;
        interval->b = ((double)i + 1) * size;
        list_append(intervals, (ListValue)interval);
    }

    for(;;) {
        // Receive any tag
        MPI_Recv(&msg, mpi_dt_message, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        switch (status.MPI_TAG) {
            //if tag is get_interval, send next interval (protect buffer?)
            case TAG_WANT_INTERVAL:
                int from = status.MPI_SOURCE;
                if (list_size(intervals) == 0) { //intervals empty, put in waiting
                    waiting[from] = WAITING;
                } else { //not empty, send interval
                    Interval* itv = (Interval*)list_pop_first(intervals);
                    msg.a = itv->a;
                    msg.b = itv->b;
                    msg.res = 0.0;
                    MPI_Send(&msg, 1, mpi_dt_message, from, TAG_RECEIVE_INTERVAL, MPI_COMM_WORLD);
                }
                break;
            //if tag is partial_result, sum to partial result variable and sum the intervals
            case TAG_PARTIAL_RESULT:
                int from = status.MPI_SOURCE;
                result += msg.res;
                splits--;
                break;
            //if tag is new_interval, insert new_interval in intervals list
            case TAG_NEW_INTERVAL:
                int from = status.MPI_SOURCE;
                Interval* itv;
                MALLOC(itv, Interval);
                itv->a = msg.a;
                itv->b = msg.b;
                list_append(intervals, (ListValue)itv);
                splits++;
                break;
            default:
                fprintf(stderr, "Node %d sent unknown tag %d\n", status.MPI_SOURCE, status.MPI_TAG);
                break;
        }
        //if has someone waiting, send interval is intervals not empty
        for (int i = 0; i < no_processes; i++) {
            if (waiting[i] == WAITING && list_size(intervals) != 0) {
                Interval* itv = (Interval*)list_pop_first(intervals);
                msg.a = itv->a;
                msg.b = itv->b;
                msg.res = 0.0;
                MPI_Send(&msg, 1, mpi_dt_message, i, TAG_RECEIVE_INTERVAL, MPI_COMM_WORLD);
            }
        }

        //if all intervals were calculated, send message to all nodes to terminate
        if (splits == 0) {
            break;
        }
    }

    printf("Result %.25f\n", result);
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
