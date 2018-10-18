/**/

#include <assert.h>
#include <math.h>
#include <mpi.h>
#include <signal.h>
#include <stddef.h> //ofsetof
#include <stdio.h>

#include "../../area.h"
#include "../list.h"

#include "../../weird.c"

#define EPSILON 0.0000000000000001
#define NOT_WAITING 0
#define WAITING 1

// TAGS
#define TAG_WANT_INTERVAL     3 //Worker node wants interval from master
#define TAG_PARTIAL_RESULT    5 //Worker node sends partial result to master
#define TAG_NEW_INTERVAL      7 //Worker node sends new interval to master
#define TAG_RECEIVE_INTERVAL  9 //Master sends new interval to calculating node
#define TAG_KILL_YOURSELF    11 //End worker nodes

#define MASTER 0

#define SEND(m, tag, to) \
    MPI_Send(&m, 1, mpi_dt_message, to, tag, MPI_COMM_WORLD)

#define RECEIVE(m, tag, from, s) \
    MPI_Recv(&m, 1, mpi_dt_message, from, tag, MPI_COMM_WORLD, &s)

typedef struct Message {
    double a;
    double b;
    double res;
} Message;

static void master(double, double);
static void worker(void);

// static const char* interval_stringfy(void* value) {
//     Interval* interval = (Interval*)value;
//     char* str;
//     MALLOC_ARRAY(str, char, 100);
//     sprintf(str, "[%f, %f]", interval->a, interval->b);
//     return str;
// }

void intHandler(int dummy) {
    MPI_Abort(MPI_COMM_WORLD, 2);
    exit(10);
}

/* Global variables */
int rank;
int no_processes; //global so master can access to create waiting_queue
int init_interval = 0;
int end_interval = 10;
MPI_Datatype mpi_dt_message;

/* Function to calculate area */
Function f = exp;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    signal(SIGINT, intHandler);
    assert(argc == 3);
    double lower_bound = strtod(argv[1], NULL);
    double upper_bound = strtod(argv[2], NULL);

    MPI_Comm_size(MPI_COMM_WORLD, &no_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Create new mpi type for struct Message
    int lengths[] = {1, 1, 1};
    MPI_Aint offsets[] = {
        offsetof(Message, a),
        offsetof(Message, b),
        offsetof(Message, res)
    };
    MPI_Datatype datatypes[] = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};
    MPI_Type_create_struct(3, lengths, offsets, datatypes, &mpi_dt_message);
    MPI_Type_commit(&mpi_dt_message);

    if (rank == MASTER) {
        master(lower_bound, upper_bound);
    } else { /* Worker nodes */
        worker();
    }

    MPI_Finalize();

    return 0;
}

static void master(double lower_bound, double upper_bound) {
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

    double size = (upper_bound - lower_bound) / (no_processes - 1);
    for (int i = 0; i < no_processes - 1; i++) {
        MALLOC(interval, Interval);
        interval->a = lower_bound + i * size;;
        interval->b = lower_bound + (i + 1.0) * size;
        list_append(intervals, (ListValue)interval);
    }

    for(;;) {
        // Receive any tag
        RECEIVE(msg, MPI_ANY_TAG, MPI_ANY_SOURCE, status);

        switch (status.MPI_TAG) {
            case TAG_WANT_INTERVAL: {
                int from = status.MPI_SOURCE;
                if (list_size(intervals) == 0) { //intervals empty
                    waiting[from] = WAITING;
                } else { //not empty
                    Interval* itv = (Interval*)list_pop_first(intervals);
                    msg.a = itv->a;
                    msg.b = itv->b;
                    msg.res = 0.0;
                    SEND(msg, TAG_RECEIVE_INTERVAL, from);
                }
                break;
            }
            case TAG_PARTIAL_RESULT:
                result += msg.res;
                splits--;
                break;
            default:
                fprintf(stderr, "Node %d sent unknown tag %d\n",
                    status.MPI_SOURCE, status.MPI_TAG);
                break;
        }

        //if someone is waiting, send interval if intervals is not empty
        for (int i = 0; i < no_processes; i++) {
            if (list_size(intervals) == 0) {
                break;
            }
            if (waiting[i] == WAITING) {
                Interval* itv = (Interval*)list_pop_first(intervals);
                msg.a = itv->a;
                msg.b = itv->b;
                msg.res = 0.0;
                SEND(msg, TAG_RECEIVE_INTERVAL, i);
            }
        }

        //if all intervals were calculated, send message to all nodes to
        //terminate
        if (splits == 0) {
            break;
        }
    }

    printf("Result %.25f\n", result);
    for (int i = 1; i < no_processes; i++) {
        SEND(msg, TAG_KILL_YOURSELF, i);
    }

    return;
}

static void worker(void) {
    Message msg;
    MPI_Status status;

    for(;;) {
        SEND(msg, TAG_WANT_INTERVAL, MASTER);
        RECEIVE(msg, MPI_ANY_TAG, MASTER, status);

        switch (status.MPI_TAG) {
            case TAG_RECEIVE_INTERVAL: {
                double result = calculate_area_recursively(f, msg.a, msg.b, f(msg.a), f(msg.b), EPSILON);
                msg.res = result;
                SEND(msg, TAG_PARTIAL_RESULT, MASTER);
                break;
            }
            case TAG_KILL_YOURSELF:
                return;
        }
    }

    return;
}
