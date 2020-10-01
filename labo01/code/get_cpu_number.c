
#define _GNU_SOURCE

#include <sched.h>

#include <stdio.h>
#include <time.h>

int main(void){
    int proc = -1;
    while(1){
        int newProc = sched_getcpu();
        if(proc != newProc){
            printf("on processor %d at time %jd\n", newProc, time(NULL));
            proc = newProc;
        }
    }
}