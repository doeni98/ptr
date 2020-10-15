#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <inttypes.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>

#define EXECUTION_TIME 10
#define PERIOD_MS 100



int main(void){
    struct timespec lastOccTime;
    struct timespec actOccTime;
    struct timespec waittime;
    waittime.tv_sec  = 0;
    waittime.tv_nsec = PERIOD_MS * 1e6;

    time_t start_time = time(NULL);
    clock_gettime(CLOCK_REALTIME, &lastOccTime);
	//unsigned long nb_iterations = 0;
	while (time(NULL) < (start_time + EXECUTION_TIME)) {
		nanosleep(&waittime, NULL);

        /* get time */
        clock_gettime(CLOCK_REALTIME, &actOccTime);

        /* calculate time difference between last cycle */
        long long timediff_s = actOccTime.tv_sec - lastOccTime.tv_sec;
        long long timediff_ns = actOccTime.tv_nsec - lastOccTime.tv_nsec;
        unsigned long long timediff = (timediff_s * 1e9) + timediff_ns ;

        fprintf(stdout ,"cycle came after %lld nanoseconds\n", timediff);

        clock_gettime(CLOCK_REALTIME, &lastOccTime);

	}
}