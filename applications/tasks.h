#include <board.h>
#include <rtdevice.h>
#include <rtthread.h>
#include <pthread.h>
#include <signal.h>
#include "sensor.h"
#include <stdlib.h>

/*Define Priority of tasks, stack size, time slice */
#define PRIORITY 10
#define STACK_SIZE 2048
#define TIME_SLICE 5

/*Period of Tasks and their worst case execution times (these are approximation)*/
#define SIM_VALUES

#ifdef SIM_VALUES
#define Ta 100      // expressed in ms
#define Tb 200
#define Tc 1000
#define Td 100
#define Te 1000
#define WCET_a 20   // expressed in ms
#define WCET_b 50
#define WCET_c 50
#define WCET_d 20
#define WCET_e 50
#endif

#define TASK_A_TRIGGER_EVENT1 (1U << 0)
#define TASK_B_TRIGGER_EVENT1 (1U << 0)
#define TASK_C_TRIGGER_EVENT1 (1U << 0)

/*Definition of buffer size for the collection of sensed value and their average */
#define BUFFER_SIZE 10
#define HISTORY_SIZE 4

/*Definition of a new type: d_occurence*/
    typedef struct {
        int occurrence;
        int  at_time;
        int hr;
        int last_values[BUFFER_SIZE];
    } d_occurence;

/*Global variables */
    int tick_start_task = 0;
    int tick_end_task = 0;
    int exec_time = 0;
    int buffer[BUFFER_SIZE];
    d_occurence history[HISTORY_SIZE];  //array of d_occurence elements
    int h = 0;                          //used by threadD as index for history
    int tot_d_occ = 0;
    int heart_r = 0;

/*Mutex */
    pthread_mutex_t history_mutex = PTHREAD_MUTEX_INITIALIZER ;

/*Events */
    static rt_event_t threadA_event;
    static rt_event_t threadB_event;
    static rt_event_t threadC_event;

/*Timers */
    static rt_timer_t timer_threadA;
    static rt_timer_t timer_threadB;
    static rt_timer_t timer_threadC;

/*Prototypes of threads entry functions */
    static void thread_A_entry(void *param);
    static void thread_B_entry(void *param);
    static void thread_C_entry(void *param);
    static void thread_D_entry(void *param);
    static void thread_E_entry(void *param);

/*Signal handler declaration for thread E*/
    void threadE_signal_handler(int sig);

/*Hook scheduler for debug */
    static void hook_of_scheduler(struct rt_thread* from, struct rt_thread* to);

/*Static Threads initialization*/
    static rt_thread_t thread_A = RT_NULL;
    static rt_thread_t thread_B = RT_NULL;
    static rt_thread_t thread_C = RT_NULL;
    static rt_thread_t thread_D = RT_NULL;
    static rt_thread_t thread_E = RT_NULL;

/*Timer callback functions of threads */
    static void timer_threadA_callback(void *parameter);
    static void timer_threadB_callback(void *parameter);
    static void timer_threadC_callback(void *parameter);

