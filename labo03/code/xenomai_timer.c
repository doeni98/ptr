#include <rtdk.h>
#include <native/task.h>
#include <native/timer.h>



void task_func(void *arg){

    
}


int main(void){
    RT_TASK mytask;

    if(mlockall(MCL_CURRENT|MCL_FUTURE)){
        rt_printf("mlockall failed\n");
        exit(1);
    }

    if(rt_task_spawn(mytask, "_mytask", 0, 50, 0, &task_func, 0)){
        rt_printf("spawn failed\n");
        exit(1);
    }

    while(1);

}