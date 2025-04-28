/* ********************************************************** *
 * CS314 Project #3 solution                                  *
 *                                                            *
 * Your last-three: 503                                       *
 * Your course section #: 001                                 *
 *                                                            *
 * Spring 2025                                                *
 *                                                            *
 * ********************************************************** */

#define SLOWDOWN_FACTOR    6000000     /* child thread display slowdown */
#define SCHEDULE_INTERVAL  1           /* scheduling interval in second */

#include <string.h>                     /* for strerror()                */
#include <stdio.h>                      // for printf ()                 
#include <signal.h>                     // for interrupt handling     
#include <unistd.h>                     // for "alarm"                 
#include <pthread.h>                    // pthread implementation      

void child_thread_routine (void * arg);
void clock_interrupt_handler(void);

pthread_mutex_t  condition_mutex;  // pthread mutex_condition 
pthread_cond_t   t_condition;      // pthread condition
int schedule_vector[5];            // the required thread schedule vector
int loop_counter;                  // loop counter for the interrupt handler

int main(void)
{
    pthread_t        thread;       // thread IDs (assigned by OS)
    pthread_attr_t     attr;       // thread attributes 
    int                 ids;       // thread arguments

    schedule_vector[0] = 0;        // the first thread
    schedule_vector[1] = 1;        // the second thread
    schedule_vector[2] = 2;        // the third thread
    schedule_vector[3] = 3;        // the fourth thread
    schedule_vector[4] = 4;        // the fifth thread
    int i = 0;                     // the loop counter for the parent thread  
    loop_counter = 0;              // initialize the loop counter for the interrupt handler

    /* specify the clock interrupt to be sent to this process --- */
    signal(SIGALRM, (void *) (clock_interrupt_handler));
    /* set the interrupt interval to 1 second --- */
    alarm(SCHEDULE_INTERVAL);

    pthread_mutex_init (&condition_mutex, NULL);
    pthread_cond_init (&t_condition, NULL);

    ids = 0;                         // thread ID = 0
    pthread_create (                 // xreate a child thread   
               &thread,              // thread ID (system assigned)
               NULL,                 // default thread attributes
               (void *) (child_thread_routine), // thread routine name
               &ids);                // arguments to be passed

    printf("   the parent thread starts looping ....\n");
    while (1)
    {
       sleep (1); // wait for one second

       printf("        the parent thread signals the child thread ...\n");
       pthread_mutex_lock(&condition_mutex);
         pthread_cond_signal(&t_condition);
       pthread_mutex_unlock(&condition_mutex);
    }
    return(0); // parent thread terminates
}

void child_thread_routine (void * arg)  
{
   int  myid = *(int *) arg;   // child thread number (not ID)
   int  my_counter = 0;        // local loop counter

   /* -------------------------------------------------------- */
   /* if you would like to add your local variable(s), you can */
   /* add one (them) here.                                     */
   /* -------------------------------------------------------- */

   /* declarare the start of this child thread (required) ---- */
   printf("Child thread %d started ...\n", myid);   

   /* -------------------------------------------------------- */
   /* if you would like to peform some initialization to your  */
   /*  local variable(s), you can do so here.                  */
   /* -------------------------------------------------------- */

   /* infinite loop (required) ------------------------------- */
   while (1)
   {
      /* declare working of myself (required) ---------------- */
      my_counter ++;
      if ((my_counter % SLOWDOWN_FACTOR) == 0)
      {  printf("Thread: %d is running ...\n", myid);  }

   /* -------------------------------------------------------- */
   /* Here is your working space (to implement some mechanism) */
   /* to synchronize with the interrupt handeler.              */
   /* -------------------------------------------------------- */
   }
}

/* The interrupt handler for SIGALM interrupt ---------------------- */
void clock_interrupt_handler(void)
{
   /* to be displayed at each time I woke up ---------------- */
   printf("I woke up on the timer interrupt (%d) .... \n", loop_counter);

   /* increase the loop counter for the interrupt handler --- */
   loop_counter = loop_counter + 1;

   /* scheduler wakes up again one second later -------------------- */  
   alarm(SCHEDULE_INTERVAL);
}