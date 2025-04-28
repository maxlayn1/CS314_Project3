/* ********************************************************** *
 * CS314 Project #3 solution                                  *
 *                                                            *
 * Your last-three: 503                                       *
 * Your course section #: 001                                 *
 *                                                            *
 * Spring 2025                                                *
 *                                                            *
 * ********************************************************** */

#define SLOWDOWN_FACTOR 6000000 /* child thread display slowdown */
#define SCHEDULE_INTERVAL 1     /* scheduling interval in second */
#define THREAD_COUNT 5          // number of threads

#include <string.h>  /* for strerror()                */
#include <stdio.h>   // for printf ()
#include <signal.h>  // for interrupt handling
#include <unistd.h>  // for "alarm"
#include <pthread.h> // pthread implementation

void *child_thread_routine(void *arg);
void clock_interrupt_handler(int signum);

pthread_mutex_t condition_mutex; // pthread mutex_condition
pthread_cond_t t_condition;      // pthread condition
pthread_barrier_t barrier;       // barrier to sync threads
int schedule_vector[5];          // the required thread schedule vector
int loop_counter;                // loop counter for the interrupt handler
int thread_count = 0;
int next_index = 0;
int current_scheduled_thread = -1;

int main(void)
{
    pthread_t threads[THREAD_COUNT];   // thread IDs (assigned by OS)
    pthread_attr_t attr[THREAD_COUNT]; // thread attributes
    int ids[THREAD_COUNT];             // thread arguments

    schedule_vector[0] = 0; // the first thread
    schedule_vector[1] = 1; // the second thread
    schedule_vector[2] = 2; // the third thread
    schedule_vector[3] = 3; // the fourth thread
    schedule_vector[4] = 4; // the fifth thread

    int parent_loop_counter = THREAD_COUNT; // the loop counter for the parent thread
    int interrupt_loop_counter = 0;         // initialize the loop counter for the interrupt handler

    signal(SIGALRM, clock_interrupt_handler); // specify the clock interrupt to be sent to this process
    alarm(SCHEDULE_INTERVAL);                 // set the interrupt interval to 1 second

    pthread_mutex_init(&condition_mutex, NULL);
    pthread_cond_init(&t_condition, NULL);
    pthread_barrier_init(&barrier, NULL, THREAD_COUNT); // initialize barrier to sync 5 threads

    for (int i = 0; i < parent_loop_counter; i++)
    { // create the 5 threads
        ids[i] = i;
        pthread_create(&threads[i], NULL, child_thread_routine, &ids[i]);
    }

    while (1)
    {
        sleep(1);
    }           // infinite wait
    return (0); // parent thread terminates
}

void *child_thread_routine(void *arg)
{
    int myid = *(int *)arg; // child thread number (not ID)
    int my_counter = 0;     // local loop counter

    /* declare the start of this child thread (required) ---- */
    thread_count = thread_count + 1;
    printf("Child thread %d started ...\n", myid);

    pthread_barrier_wait(&barrier); // wait until all threads reach this point

    while (1)
    {
        pthread_mutex_lock(&condition_mutex);
        while (myid != current_scheduled_thread)
        {
            pthread_cond_wait(&t_condition, &condition_mutex);
        }

        /* infinite loop (required) ------------------------------- */
        while (1)
        {
            if (current_scheduled_thread != myid)
            {
                break;
            }

            /* declare working of myself (required) ---------------- */
            my_counter++;
            if ((my_counter % SLOWDOWN_FACTOR) == 0)
            {
                printf("TEST-Thread: %d is running ...\n", myid);
            }
        }
    }
}

/* The interrupt handler for SIGALM interrupt ---------------------- */
void clock_interrupt_handler(int signum)
{
    printf("\nI woke up on the timer interrupt (%d) .... \n", loop_counter);

    current_scheduled_thread = schedule_vector[next_index]; // id of thread at schedule_vector position
    next_index = (next_index + 1) % THREAD_COUNT;           // next index of schedule_vector

    pthread_cond_broadcast(&t_condition);   // wake up all waiting threads, but only the scheduled one will make it past the loop
    pthread_mutex_unlock(&condition_mutex); // allow threads to enter the waiting loop first time interrupt handler goes off

    loop_counter = loop_counter + 1; // increase loop counter for interrupt handler
    alarm(SCHEDULE_INTERVAL);        // schedule next one second timer
}