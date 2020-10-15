#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>


timer_t timer;
struct itimerspec spec;

int nbMesure = 0;
struct timespec *alarmTimes;
long long nsec = 0;

void timer_handler(int signum)
{
    static int count = 0;
    
     if(count == nbMesure){
        #ifdef DEBUG
        fprintf(stdout, "first timer came at %ld.%9ld\n", alarmTimes[0].tv_sec, alarmTimes[0].tv_nsec);
        #endif
        for(int i = 1; i < nbMesure; ++i){
            long long timediff_s = alarmTimes[i].tv_sec - alarmTimes[i-1].tv_sec; 
            long long timediff_ns = alarmTimes[i].tv_nsec - alarmTimes[i-1].tv_nsec;
            unsigned long long timediff = (timediff_s * 1e9) + timediff_ns ;
            #ifdef DEBUG
            fprintf(stdout, "alarm %d came at %ld.%9ld and had a interval to the timer before of %lldns, which is %lldns to late\n",
                    i, alarmTimes[i].tv_sec, alarmTimes[i].tv_nsec, timediff, timediff - nsec);
            #else
            fprintf(stdout, "%lld\n", timediff - nsec);
            #endif
        }
        exit(0);
    }else{
        clock_gettime(CLOCK_REALTIME, alarmTimes + count);
    }
    count++;
}

int main(int argc, char **argv){
    int microsec;
    struct sigevent event;
    

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s NB_MESURES INTERVAL_micros\n", argv[0]);
        return EXIT_FAILURE;
    }

    nbMesure = atoi(argv[1]);
    microsec = atoi(argv[2]);

    #ifdef DEBUG
    fprintf(stdout, "init application with %d mesurements and interval of %dmicros\n", nbMesure, microsec);
    #endif

    //allouer de la memoire pour les resultat
    alarmTimes = calloc(nbMesure, sizeof(struct timespec));
    if(alarmTimes == NULL){
        fprintf(stderr, "Error on Memory Allocation");
        return EXIT_FAILURE;
    }

    // Configurer le timer
    if(signal(SIGRTMIN, timer_handler)){
        fprintf(stderr, "Error on signal function");
        return EXIT_FAILURE;
    }


    event.sigev_notify = SIGEV_SIGNAL;
    event.sigev_signo = SIGRTMIN;
    nsec = microsec * 1e3; // en nanosec
    spec.it_interval.tv_sec = nsec / 1e9;
    spec.it_interval.tv_nsec = nsec % (int)1e9;
    spec.it_value = spec.it_interval;

    // Allouer le timer
    if(timer_create(CLOCK_REALTIME, &event, &timer)){
        fprintf(stderr, "Error on signal timer_create");
        return EXIT_FAILURE;
    }

    // Programmer le timer
    if(timer_settime(timer, 0, &spec, NULL)){
        fprintf(stderr, "Error on signal timer_settime");
        return EXIT_FAILURE;
    }
    
    while(1);
}