
/**
* \file watchdog.c
* \brief Fonctions permettant la mise en place d'un watchdog.
* \author Y. Thoma & D. Molla edited: Denis Bourqui 
* \version 0.2
* \date 12 novembre 2020
* 
* Le watchdog proposé est implémenté à l'aide de 2 tâches responsables
* de détecter une surcharge du processeur. Si tel est le cas, une fonction
* fournie par le développeur est appelée. Elle devrait suspendre ou détruire
* les tâches courantes, afin de redonner la main à l'OS.
*
* Code Xenomai 3.0
*
* Explication sur l'implementation:
* Le Canary a une periode de 1s et une priorite de 5. Le Watchdog une periode de 2s et une priorite de 99.
* Deux semaphores sont utilise en tant que barriere pour que les deux taches commencent le plus possible en meme temps.
* Le Canary incremente une valeur de compteur a chaque cycle. Le Watchdog surveille cette incrementation. Si entre deux
* execution du Watchdog, l'incrementation ne sait pas fait, alors le systeme est surchage et la fonction suspend est appelle.
* La fonction end_watchdog tue les deux taches pour bien nettoyer le systeme.
*
*/

#include <alchemy/task.h>
#include <alchemy/alarm.h>
#include <alchemy/timer.h>
#include <alchemy/sem.h>

#include "watchdog.h"
#include "general.h"

#define CANARY_PERIOD 1 * 1000000000ull       /**< Period of canary task (Pb) */
#define WATCHDOG_PERIOD 2  * 1000000000ull    /**< Period of Watchdog task (Ph). 
                                                Keep in mind that following equation
                                                must allways be true: Ph > Pb */

#define CANARY_PRIO 5                         /**< Priority of Canary task (Prb). Must be low */
#define WATCHDOG_PRIO 99                      /**< Priority of Watchdog task (Prh). Must be high so that Prh > Prb */

unsigned long canary_counter = 0;             /**< Canary Counter: is incremented by canary Task */

RT_TASK canaryTask,                           /**< Canary RT_Task */
  watchdogTask;                               /**< Watchdog RT_Task */

RT_SEM waitOnCanary,                         /**< Canaray semaphore. Used by watchdog to know that canary is ready */
  waitOnWatchdog;                            /**< Watchdog semaphore. Used by Canary to know that Watchdog is ready */

void(*watchdog_suspend_function)(void) = NULL; /**< Fonction appelée lors d'une surcharge */

/** \brief Canary Function
*
* This is the function executed by the Canary Task. 
* - Wait until the Watchdog task is ready. 
* - Then it sets its period. 
* - Then a infite loop is started looping ones at every period. It increments the value of canary_counter. 
* \param args No Parameters needs to be passed here
*/
void canary(void *args){

  /* Release Watchdog */
  rt_sem_v(&waitOnCanary);
  /* Wait on Watchdog */
  rt_sem_p(&waitOnWatchdog, TM_INFINITE);
  
  /* set priod */
  if(rt_task_set_periodic(rt_task_self(), TM_NOW, rt_timer_ns2ticks(CANARY_PERIOD))){
      rt_printf("error on set period of canary\n");
      return;
  }
 
  while(1){
    /* Increment Counter */
    canary_counter++;
    /* wait on next period */
    rt_task_wait_period(NULL);
  }

}

/** \brief Watchdog Function
*
* This is the function executed by the Watchdog Task. 
* - Wait on Canary Task to be ready
* - Then it sets its period. 
* - Then a infite loop is started looping ones at every period.
*   If the canary_counter has the same value in two succesif executions,
-   then the Canary wasn't able to execute. In this case the 
-   watchdog_suspend_function function is called. 
* \param args No Parameters needs to be passed here
*/
void watchdog(void *args){
  unsigned long cnt_last = 0;
  
  /* Release Canary */
  rt_sem_v(&waitOnWatchdog);
  /* Wait on Canary */
  rt_sem_p(&waitOnCanary, TM_INFINITE);

  /* set priod */
  if(rt_task_set_periodic(rt_task_self(), TM_NOW, rt_timer_ns2ticks(WATCHDOG_PERIOD))){
      rt_printf("error on set period of canary\n");
      return;
  }

  while(1){
    /* wait on next period */
    rt_task_wait_period(NULL);

    if(cnt_last == canary_counter){
      rt_printf("Watchdog Detection\n");
      watchdog_suspend_function();
      end_watchdog();
    }

    cnt_last = canary_counter; 
  }
}

/** \brief Démarre le watchdog
*
* Le watchdog lance deux tâches. Une tâche de faible priorité qui incrémente
* un compteur, et une de priorité maximale mais de période moindre qui
* vérifie que le compteur s'est incrémenté. Si tel n'est pas le cas,
* la fonction passée en paramètre est appelée. Elle devrait contenir du code
* responsable de suspendre ou supprimer les tâches courantes.
* \param suspend_func Fonction appelée par le watchdog en cas de surcharge
*/
int start_watchdog(void(* suspend_func)(void))
{
  /* Callback function in case of canary overload detection */
  watchdog_suspend_function = suspend_func;
  
  /* Create Canary Mutex  */
  if(rt_sem_create(&waitOnCanary, "CanarySemaphore", 0, S_FIFO)){
      rt_printf("Error on Semaphore Create Canary\n");
      return 0;
  }

  /* Create Watchdog Mutex  */
  if(rt_sem_create(&waitOnWatchdog, "WatchdogSemaphore",0, S_FIFO)){
      rt_printf("Error on Semaphore Create Watchdog\n");
      return 0;
  } 

  /* create canary task  */
  if(rt_task_create (&canaryTask, "myCanary", 0, CANARY_PRIO, 0)){
      rt_printf("error on task create\n");
      return 0;
  }

  /* create watchdog task  */
  if(rt_task_create (&watchdogTask, "myWatchdog", 0, WATCHDOG_PRIO, 0)){
      rt_printf("error on task create\n");
      return 0;
  }

  /* run canary task */
  if(rt_task_start (&canaryTask, &canary, NULL)){
      rt_printf("error on task start\n");
      return 0;
  }

  /* run watchdog task */
  if(rt_task_start (&watchdogTask, &watchdog, NULL)){
      rt_printf("error on task start\n");
      return 0;
  }

  return 1;
}


/** \brief Termine les deux tâches du watchdog
*
* Cette fonction doit être appelée par la fonction de cleanup du module
* afin de libérer les ressources allouées par le watchdog.
* En l'occurence, les deux tâches du watchdog sont supprimées.
* Elle peut aussi être appelée à n'importe quel instant pour supprimer
* la surveillance du watchdog.
*/
int end_watchdog(void)
{
  rt_task_delete(&canaryTask);
  rt_task_delete(&watchdogTask);
  return 1;
}
