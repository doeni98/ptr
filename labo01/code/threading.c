#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <inttypes.h>
#include <errno.h>
#define EXECUTION_TIME 10 /* In seconds */

/* Barrier variable */
pthread_barrier_t barr;
time_t start_time;
void *f_thread(void *arg)
{
    pthread_barrier_wait(&barr);
    unsigned long nb_iterations = 0;
	while (time(NULL) < (start_time + EXECUTION_TIME)) {
		++nb_iterations;
	}
    *((unsigned long*) arg) = nb_iterations;
}

int main(int argc, char **argv)
{
    int nb_threads;
    int min_prio;
    int max_prio;
    int i;
    int *prio_value;
    unsigned long *nb_iterations;
    unsigned long total_iterations = 0;
    pthread_t *threads;

    /* Parse input */
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s NB_THREADS\n", argv[0]);
        return EXIT_FAILURE;
    }
    nb_threads = strtoimax(argv[1], (char **)NULL, 10);
    if (nb_threads <= 0)
    {
        fprintf(stderr, "NB_THREADS must be > 0 (actual: %d)\n", nb_threads);
        return EXIT_FAILURE;
    }
    /* memory allocation */
    prio_value = (int *)calloc(nb_threads, sizeof(int));
    nb_iterations = (long *)calloc(nb_threads, sizeof(unsigned long));
    threads = (long *)calloc(nb_threads, sizeof(pthread_t));

    /* Get minimal and maximal priority values */
    min_prio = sched_get_priority_min(SCHED_FIFO);
    max_prio = sched_get_priority_max(SCHED_FIFO);
    max_prio -= min_prio;
    fprintf(stdout, "minimal prio: %d, maximal prio: %d\n", min_prio, max_prio);

    /* Initialize barrier */
    if (pthread_barrier_init(&barr, NULL, nb_threads))
    {
        fprintf(stderr, "Could not initialize barrier!\n");
        return EXIT_FAILURE;
    }

    start_time = time(NULL);

    /* Set priorities and create threads */
    for(i = 0; i < nb_threads; ++i){
        
        prio_value[i] =  ((max_prio + min_prio) / nb_threads) * i + min_prio + 1;
        fprintf(stdout, "new thread init with prio %d\n", prio_value[i]);
        

        /* Set thread attributes necessary to use priorities */
        struct sched_param sched_param;
        pthread_attr_t thread_attr;
        int policy;


        
        if(pthread_attr_init(&thread_attr)){
            fprintf(stderr, "Error on pthread_attr_init\n");
        }

        if(pthread_attr_getschedparam(&thread_attr, &sched_param)){
            fprintf(stderr, "Error on pthread_attr_getschedparam\n");
        }
        
        if(pthread_attr_setschedpolicy(&thread_attr, SCHED_FIFO)){
            fprintf(stderr, "Error on pthread_attr_setschedpolicy\n");
        }

        if(pthread_attr_setinheritsched(&thread_attr, PTHREAD_EXPLICIT_SCHED)){
            fprintf(stderr, "Error on pthread_attr_setinheritsched\n");
        }
        
        sched_param.sched_priority = prio_value[i];

        if(pthread_attr_setschedparam(&thread_attr, &sched_param)){
            fprintf(stderr, "Error on pthread_attr_setschedparam with prio value %d\n", sched_param.sched_priority );
        }

        if(pthread_create(threads+i, &thread_attr, f_thread, (void *)(nb_iterations+i))){
            fprintf(stderr, "Error on pthread_create\n");
        }
         
        if(pthread_getschedparam(threads[i], &policy , &sched_param)){
            fprintf(stderr, "Error on pthread_getschedparam\n");
        }
        policy = SCHED_FIFO;
        if(pthread_setschedparam(threads[i], policy, &sched_param)){
            fprintf(stderr, "Error on pthread_setschedparam\n");
        }
        switch(pthread_setschedprio(threads[i], prio_value[i])){
            case EINVAL:
                fprintf(stderr, "prio is not valid for the scheduling policy of the specified thread.\n");
                break;
            case EPERM:
                fprintf(stderr, "The caller does not have appropriate privileges to set the specified priority.\n");
                break;
            case 0:
                fprintf(stdout, "created thread %d with prio %d, policy: %d\n", i, prio_value[i], policy);
        }

    }
  
    fprintf(stdout, "all thread started\n");
    /* Wait for the threads to complete and set the results. beginning back of the threads array. (from high prio to low prio thread) */
    for(i = nb_threads -1 ; i >= 0; --i){
        fprintf(stdout, "waiting on thread number %d\n", i);
        pthread_join(threads[i], NULL);
    }

    fprintf(stdout, "All threads ended in %d seconds\n",time(NULL) -  start_time );
    /* summing iterations */
    for(i = 0; i < nb_threads; ++i){
        total_iterations += nb_iterations[i];
    }

    for (i = 0; i < nb_threads; ++i)
    {
        fprintf(stdout, "[%02d] %ld (%2.0f%%)\n",
                prio_value[i], nb_iterations[i],
                100.0 * nb_iterations[i] / total_iterations);
    }
    getchar();
    return EXIT_SUCCESS;
}