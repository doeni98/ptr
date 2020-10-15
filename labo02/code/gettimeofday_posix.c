#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NB_MESURES 30


int main(int argc, char **argv)
{
    struct timespec tp[NB_MESURES];
    int i;
    for (i = 0; i < NB_MESURES; ++i)
    {
        // get time of realtime clock
        clock_gettime(CLOCK_REALTIME, tp + i);
    }

    for(i = 0; i < NB_MESURES;++i){
        // Print times in nano seconds
        printf("%2d : %ld.%9ld\n", i, tp[i].tv_sec, tp[i].tv_nsec);
    }
    return EXIT_SUCCESS;
}