/**/

#include <assert.h>
#include <math.h>
#include <mpi.h>
#include <signal.h>
#include <stddef.h> //ofsetof
#include <stdio.h>

#include "../area.h"
#include "list.h"

#include "../weird.c"

#define EPSILON 0.0000000000000001
#define NOT_WAITING 0
#define WAITING 1

// TAGS
#define TAG_WANT_INTERVAL    3//Calculating node wants interval from master
#define TAG_PARTIAL_RESULT   5//Calculating node sends partial result to master
#define TAG_NEW_INTERVAL     7//Calculating node sends new interval to master
#define TAG_RECEIVE_INTERVAL 9//Master sends new interval to calculating node

#define MASTER 0

#define SEND(m, tag, to) \
    MPI_Send(&m, 1, mpi_dt_message, to, tag, MPI_COMM_WORLD)

#define RECEIVE(m, tag, s) \
    MPI_Recv(&m, 1, mpi_dt_message, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &s)

typedef struct Message {
    double a;
    double b;
    double res;
} Message;

static void master(double, double);
static void worker(void);

static const char* interval_stringfy(void* value) {
    Interval* interval = (Interval*)value;
    char* str;
    MALLOC_ARRAY(str, char, 100);
    sprintf(str, "[%f, %f]", interval->a, interval->b);
    return str;
}

void intHandler(int dummy) {
    MPI_Abort(MPI_COMM_WORLD, 2);
    exit(10);
}

/* Global variables */
int rank;
int no_processes; //need to be global so master can access to create waiting_queue
int init_interval = 0;
int end_interval = 10;
MPI_Datatype mpi_dt_message;

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

    if (rank == 0) { /* Master node */
        master(lower_bound, upper_bound);
    } else { /* Calculating nodes */
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

    list_dump(intervals, interval_stringfy);

    for(;;) {
        // Receive any tag
        MPI_Recv(&msg, 1, mpi_dt_message, MPI_ANY_SOURCE, MPI_ANY_TAG,
            MPI_COMM_WORLD, &status);

        switch (status.MPI_TAG) {
            //if tag is get_interval, send next interval (protect buffer?)
            case TAG_WANT_INTERVAL: {
                int from = status.MPI_SOURCE;
                if (list_size(intervals) == 0) { //intervals empty, put in waiting
                    waiting[from] = WAITING;
                } else { //not empty, send interval
                    Interval* itv = (Interval*)list_pop_first(intervals);
                    msg.a = itv->a;
                    msg.b = itv->b;
                    msg.res = 0.0;
                    // printf("send %f %f %f\n", msg.a, msg.b, msg.res);
                    MPI_Send(&msg, 1, mpi_dt_message, from,
                        TAG_RECEIVE_INTERVAL, MPI_COMM_WORLD);
                }
                break;
            }
            //if tag is partial_result, sum to partial result variable and sum
            //the intervals
            case TAG_PARTIAL_RESULT:
                result += msg.res;
                splits--;
                break;
            //if tag is new_interval, insert new_interval in intervals list
            case TAG_NEW_INTERVAL: {
                Interval* itv;
                MALLOC(itv, Interval);
                itv->a = msg.a;
                itv->b = msg.b;
                list_append(intervals, (ListValue)itv);
                splits++;
                break;
            }
            default:
                fprintf(stderr, "Node %d sent unknown tag %d\n",
                    status.MPI_SOURCE, status.MPI_TAG);
                break;
        }
        //if has someone waiting, send interval is intervals not empty
        for (int i = 0; i < no_processes; i++) {
            if (waiting[i] == WAITING && list_size(intervals) != 0) {
                Interval* itv = (Interval*)list_pop_first(intervals);
                msg.a = itv->a;
                msg.b = itv->b;
                msg.res = 0.0;
                MPI_Send(&msg, 1, mpi_dt_message, i, TAG_RECEIVE_INTERVAL,
                    MPI_COMM_WORLD);
            }
        }

        // printf("splits: %d\n", splits);

        //if all intervals were calculated, send message to all nodes to
        //terminate
        if (splits == 0) {
            break;
        }
    }

    printf("Result %.25f\n", result);
    MPI_Abort(MPI_COMM_WORLD, 0);

    return;
}

static void worker(void) {
    Interval* interval = NULL;
    Message msg;
    MPI_Status status;

    for(;;) {
        if (!interval) {
            SEND(msg, TAG_WANT_INTERVAL, MASTER);
            RECEIVE(msg, TAG_RECEIVE_INTERVAL, status);
            MALLOC(interval, Interval);
            interval->a = msg.a;
            interval->b = msg.b;
        }
        double result = calculate_area_partially(exp, interval, EPSILON);

        if (isnan(result)) {
            double c = (interval->b + interval->a) / 2.0;
            msg.a = c;
            msg.b = interval->b;
            SEND(msg, TAG_NEW_INTERVAL, MASTER);
            interval->b = c;
        } else {
            msg.res = result;
            SEND(msg, TAG_PARTIAL_RESULT, MASTER);
            free(interval);
            interval = NULL;
        }
    }

    return;
}
