#include <rtdk.h>
#include <native/task.h>
#include <native/timer.h>
#include <sys/mman.h>


#define TIMESLEEP 100000000 /* 0.1 sec */
#define NB_PERIODS 15

RTIME starttime;
RT_TASK mytask;

void task_func(void *arg){
    RTIME now;
    RTIME last;

    /* set priod */
    if(rt_task_set_periodic(rt_task_self(), TM_NOW, rt_timer_ns2ticks(TIMESLEEP))){
        rt_printf("error on set period\n");
        return;
    }

    last = rt_timer_read();
    for(int i = 0; i < NB_PERIODS; ++i){
        // get time
        now = rt_timer_read();

        // calculate delta
        int delta  = now - last - TIMESLEEP;
        rt_printf("time: %lluns, delta %d: %dns\n", now, i, delta);
        last = now;

        //wait on next period
        rt_task_wait_period(NULL);
    }

}


int main(void){
    rt_printf("go\n");

    // init time 0
    starttime = rt_timer_read();

    // taskname
    const char *taskname = "_mytask";

    // memory lock
    if(mlockall(MCL_CURRENT|MCL_FUTURE)){
        rt_printf("mlockall failed\n");
        exit(1);
    }

    rt_print_auto_init(1);

    // create joinable task running with high priority
    rt_printf("create\n");
    if(rt_task_create (&mytask, taskname, 0, 90, T_JOINABLE)){
        rt_printf("error on task create");
    }

    // run task
    rt_printf("start\n");
    if(rt_task_start (&mytask, &task_func, NULL)){
        rt_printf("error on task start");
    }

    // wait on task to finish
    rt_printf("wait on join\n");
    rt_task_join(&mytask);
    rt_printf("joined\n");

}