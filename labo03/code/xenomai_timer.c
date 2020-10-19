#include <rtdk.h>
#include <native/task.h>
#include <native/timer.h>

#define PERIOD 1000

RTIME starttime;

void task_func(void *arg){
    /* set priod */
    rt_task_set_periodic(rt_task_self(), TM_NOW, PERIOD);
    
    RTIME now;
    RTIME last = rt_timer_read();
    while(){
        now = rt_timer_read();

        rt_printf("diff %d", now - last);
        rt_task_wait_period(NULL);
    }

}


int main(void){
    RT_TASK mytask;
    starttime = rt_timer_read();

    if(mlockall(MCL_CURRENT|MCL_FUTURE)){
        rt_printf("mlockall failed\n");
        exit(1);
    }

    rt_print_auto_init(1);

    

    if(rt_task_spawn(mytask, "_mytask", 0, 50, 0, &task_func, 0)){
        rt_printf("spawn failed\n");
        exit(1);
    }


    while(1);

}