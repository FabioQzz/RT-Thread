#include "tasks.h"
#include "sensing.c"

/* Entry Function for each Threads*/
static void thread_A_entry(void *param){

    int dx_mV, sx_mV;      //sensed values of electrical potential
    int avg_current = 0;   //average of the 2 sensed values dx and sx
    int cnt = 0;
    int i, avg_buffer, delta;
    int danger = 0;

    while(1){
    /*Every Ta[ms] (period) the task must be scheduled */
        rt_event_recv(threadA_event, TASK_A_TRIGGER_EVENT1, RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, RT_NULL);
        tick_start_task = rt_tick_get_millisecond();
        rt_kprintf("--------------------------------------------------------------------\n");
        rt_kprintf("Task A running: sensing electrical potential of atrium cells\n");
        danger = 0;

   /*Sensing the electrical potential of the atrium cells: dx and sx*/
        dx_mV = read_sensor_dx();
        sx_mV = read_sensor_sx();
        rt_kprintf("The electrical potential of atrium sx is=%d\nThe electrical potential of atrium dx is=%d\n", sx_mV,  dx_mV); //1 e 5

   /*Computing the average of the two just sensed values and storing to the buffer */
       avg_current = (dx_mV + sx_mV)/2;
       buffer[cnt % BUFFER_SIZE] = avg_current;

   /*Computing the average of the last 10 values in the buffer and comparison with avg_current  */
       if(cnt >= BUFFER_SIZE){
           for (i = 0; i < BUFFER_SIZE; i++)
                avg_buffer += buffer[i];

       avg_buffer = avg_buffer / BUFFER_SIZE;
       rt_kprintf("Average Buffer=%d, Current Value Average=%d\n", avg_buffer, avg_current);

   /*Check if the average of the buffer differs too much from the current average, in case it does trigger TaskD */
       delta = abs(avg_buffer - avg_current);
       if (delta > 30) {
           rt_kprintf("Attention! Delta = %d >30%\n", delta);
           danger = 1;
           }
       }

       cnt++;

       if(danger){
            rt_kprintf("--------------------------------------------------------------------\n");
            rt_kprintf("Danger! Activating task D,the sensed value is too different from the Average!\n");
            thread_D = rt_thread_create("threadD", thread_D_entry, RT_NULL, 512, 1, 5);
            rt_thread_startup(thread_D);
            rt_thread_delete(thread_D);
        }

   /*Computing the execution time of the task */
        tick_end_task = rt_tick_get_millisecond();
        exec_time = tick_end_task - tick_start_task;
        rt_kprintf("Task A execution time is: %d \n ", exec_time);
        rt_kprintf("Flag_sens=%d \n ", flag_sens);

    /*Checking WCET constraint */
        if(exec_time > WCET_a){
            rt_kprintf("Attention WCET of task A exceeded of  %d ms \n ", exec_time-WCET_a);
        }
        rt_kprintf("--------------------------------------------------------------------\n");
    }
}


static void thread_B_entry(void *param){
    while(1){

    /*Every Tb[ms] (period) the task must be scheduled */
        rt_event_recv(threadB_event, TASK_B_TRIGGER_EVENT1, RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, RT_NULL);
        tick_start_task = rt_tick_get_millisecond();
        rt_kprintf("--------------------------------------------------------------------\n");
        rt_kprintf("Task B running: sensing the heart beats\n");
        heart_r = read_heart_rate();
        rt_kprintf("The current heartbeats rate is=%d HB/min\n",heart_r );
    /*Computing the execution time of the task */
        tick_end_task = rt_tick_get_millisecond();
        exec_time = tick_end_task - tick_start_task;
        rt_kprintf("Task B execution time is: %d \n ", exec_time);

     /*Checking WCET constraint */
        if(exec_time > WCET_b){
            rt_kprintf("Attention WCET of task B exceeded of  %d ms \n ", exec_time-WCET_b);
        }
        rt_kprintf("--------------------------------------------------------------------\n");
    }
}

/* Signal process function for thread  signal handler */
void threadE_signal_handler(int sig)
{       rt_kprintf("--------------------------------------------------------------------\n");
        rt_kprintf("Task C running: received signal %d from the User\nLaunching thread E\n", sig);

    /* Create thread E */
        thread_E = rt_thread_create("threadE",thread_E_entry, RT_NULL, 1024, 8, 5);
        if (thread_E != RT_NULL){
               rt_thread_startup(thread_E);
        }
        rt_thread_delete(thread_E);
}


static void thread_C_entry(void *param){

     while(1){
     /* Every Tc[ms] (period) the task must be scheduled */
             rt_kprintf("--------------------------------------------------------------------\n");

             rt_event_recv(threadC_event, TASK_C_TRIGGER_EVENT1, RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, RT_NULL);
             rt_kprintf("Task C: waiting for user request\n");

             tick_start_task = rt_tick_get_millisecond();

             rt_signal_install(SIGUSR1, threadE_signal_handler); // link SIGUSR1 with thread E execution
             rt_signal_unmask(SIGUSR1);

     /* Computing the execution time of the task */
            tick_end_task = rt_tick_get_millisecond();
            exec_time = tick_end_task - tick_start_task;
            rt_kprintf("Task C execution time is: %d \n ", exec_time);

     /* Checking WCET constraint */
            if(exec_time > WCET_c){
                  rt_kprintf("Attention WCET of task C exceeded of  %d ms \n ", exec_time-WCET_c);
            }
              rt_kprintf("--------------------------------------------------------------------\n");
                //rt_thread_delay(1000);
            }
    }


static void thread_D_entry(void *param){
        int i;
        rt_kprintf("--------------------------------------------------------------------\n");
        rt_kprintf("Task D running: leads activated\n");

        tot_d_occ++;

        pthread_mutex_lock(&history_mutex);       //lock the mutex

        history[h % HISTORY_SIZE].occurrence = tot_d_occ;
        history[h % HISTORY_SIZE].at_time = rt_tick_get_millisecond();
        history[h % HISTORY_SIZE].hr = heart_r ;
        for(i=0;i< BUFFER_SIZE; i++)
            history[h% HISTORY_SIZE].last_values[i] = buffer[i];

        pthread_mutex_unlock(&history_mutex);    //unlock the mutex

        flag_sens = 1;                          //used in sensing.c to generate values in a correct interval

  /*Print the current occurence of D and the average values in the buffer at the moment */
        rt_kprintf("Occurrence %d of threadD,at time:%d\n",history[h% HISTORY_SIZE].occurrence, history[h% HISTORY_SIZE].at_time );
        rt_kprintf("Last 10 value sensed:\n[");
        for(i=0;i< BUFFER_SIZE; i++){
            rt_kprintf("%d ",history[h% HISTORY_SIZE].last_values[i]);
        }
        h++;
        rt_kprintf("]\n");
        rt_kprintf("--------------------------------------------------------------------\n");

}

static void thread_E_entry(void *param){
        int i,j;
        rt_kprintf("--------------------------------------------------------------------\n");
        rt_kprintf("Task E running: sending data to the User\n");
        tick_start_task = rt_tick_get_millisecond();

        pthread_mutex_lock(&history_mutex);       //lock the mutex
        rt_kprintf("Sending the history to the User:\n");

        for(j=0; j < HISTORY_SIZE; j++){
            rt_kprintf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
            rt_kprintf("Occurrence number:%d of threadD occurred at time:%d [ms]\n",history[j].occurrence, history[j].at_time );
            rt_kprintf("The current heart beat at time %d[ms]is: %d HB/min\n[", history[j].at_time, history[j].hr );
            rt_kprintf("The current buffer at time:%d\n[", history[j].at_time );

            for(i=0; i < BUFFER_SIZE; i++){
                rt_kprintf("%d ",history[j].last_values[i]);
            }
            rt_kprintf("]\n");
            rt_kprintf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

        }
        pthread_mutex_unlock(&history_mutex);    //unlock the mutex

    /* Computing the execution time of the task */
        tick_end_task = rt_tick_get_millisecond();
        exec_time = tick_end_task - tick_start_task;
        rt_kprintf("Task E execution time is: %d \n ", exec_time);

    /* Checking WCET constraint */
        if(exec_time > WCET_e){
          rt_kprintf("Attention WCET of task E exceeded of  %d ms \n ", exec_time-WCET_e);
        }
        rt_kprintf("--------------------------------------------------------------------\n");

        list_thread();
}


/*To understand when we switch from a thread to another */
static void hook_of_scheduler(struct rt_thread* from, struct rt_thread* to)
{
    rt_kprintf("--------------------------------------------------------------------\n");
    rt_kprintf("Switching from thread: %s -->to: %s at tick:%d\n", from->name , to->name, rt_tick_get_millisecond());
    rt_kprintf("--------------------------------------------------------------------\n");

}


/*Timers of Threads */
static void timer_threadA_callback(void *parameter){
    rt_event_send(threadA_event, TASK_A_TRIGGER_EVENT1);
}

static void timer_threadB_callback(void *parameter){
    rt_event_send(threadB_event, TASK_B_TRIGGER_EVENT1);
}

static void timer_threadC_callback(void *parameter){
    rt_event_send(threadC_event, TASK_C_TRIGGER_EVENT1);
}
