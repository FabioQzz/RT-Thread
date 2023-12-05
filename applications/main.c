/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 * 2018-11-19     flybreak     add stm32f407-atk-explorer bsp
 */

#include "tasks.c"                          /*include the content of the file tasks.c */
#include <dfs_fs.h>
#include <rtdevice.h>

#define LED0_PIN    GET_PIN(F, 9)           /* defined the LED0 piSn: PF9 */
#define RT_DEVICE_FLAG_RDWR         0x003   /* read and write  */
#define RT_DEVICE_OFLAG_RDWR 0x003          /* open the device in read-and_write mode */
#define SIGUSR1 30                          /* user defined signal 1 */

int main(void)
{
    /* Initialize the buffer with zeros */
       memset(buffer,0,BUFFER_SIZE);

    /* Set the scheduler hook */
        rt_scheduler_sethook(hook_of_scheduler);

    /* Creation of Threads and check of correct creation */
    /* Parameters are : name of thread, entry function, parameter for the entry function, stack size, priority, time slice */

        thread_A = rt_thread_create("threadA", thread_A_entry, RT_NULL, STACK_SIZE, PRIORITY-3, TIME_SLICE);
                if(thread_A == RT_NULL){
                    rt_kprintf("Operation failed: creation of task A\n");
                }

        thread_B = rt_thread_create("threadB", thread_B_entry, RT_NULL, STACK_SIZE, PRIORITY-2, TIME_SLICE);
                if(thread_B == RT_NULL){
                    rt_kprintf("Operation failed: creation of task B\n");
                }

        thread_C = rt_thread_create("threadC", thread_C_entry, RT_NULL, STACK_SIZE, PRIORITY-1, TIME_SLICE);
                if(thread_C == RT_NULL){
                    rt_kprintf("Operation failed: creation of task C\n");
                }

        list_thread();

   /*Creation of events */
        threadA_event = rt_event_create("TaskAEvent", RT_IPC_FLAG_PRIO);
        threadB_event = rt_event_create("TaskBEvent", RT_IPC_FLAG_PRIO);
        threadC_event = rt_event_create("TaskCEvent", RT_IPC_FLAG_PRIO);

        list_event();

   /* Creation of timers  */
        timer_threadA = rt_timer_create("TaskATimer", timer_threadA_callback, RT_NULL, Ta, RT_TIMER_FLAG_PERIODIC);
        timer_threadB = rt_timer_create("TaskBTimer", timer_threadB_callback, RT_NULL, Tb, RT_TIMER_FLAG_PERIODIC);
        timer_threadC = rt_timer_create("TaskCTimer", timer_threadC_callback, RT_NULL, Tc, RT_TIMER_FLAG_PERIODIC);

       list_timer();

   /* Startup of Threads */
        rt_thread_startup(thread_A);
        rt_thread_startup(thread_B);
        rt_thread_startup(thread_C);

        rt_timer_start(timer_threadA);
        rt_timer_start(timer_threadB);
        rt_timer_start(timer_threadC);

    /* Simulating an asynchronous request from the User
    * by sending signal SIGUSR1 after 1 seconds to thread C
    * it is a sort of software interrupt  */
        rt_thread_mdelay(1000);
        rt_thread_kill(thread_C, SIGUSR1);

   /* Send signal SIGUSR1 after 10 seconds to thread C, to startup thread E */
        rt_thread_mdelay(10000);
        rt_thread_kill(thread_C, SIGUSR1);

    return RT_EOK;
}


/*
   Creating a Block device
       rt_device_t block_dev, spi_quazz;
       block_dev = rt_device_create(RT_Device_Class_Block, 128);
       spi_quazz = rt_device_create(RT_Device_Class_SPIDevice, 128);

   Registering, initializing and opening the block device
       if(rt_device_register(block_dev, "sd0", RT_DEVICE_FLAG_RDWR ) != RT_EOK)
           rt_kprintf("The Block device NOT successfully registered\n");

       if (rt_device_init(block_dev) != RT_EOK)
           rt_kprintf("Block Device NOT initialized correctly\n");

       if(rt_device_open(block_dev, RT_DEVICE_OFLAG_RDWR) != RT_EOK)
           rt_kprintf("The device is NOT open \n");

       rt_device_register(spi_quazz, "spi5", RT_DEVICE_FLAG_RDWR );
       rt_device_init(spi_quazz);

       if(dfs_mkfs("elm", "sd0") == 0)
           rt_kprintf("File system formatted successfully\n");


   Mounting an elmFat fs to the specified path '/'
       if (dfs_mount("sd0", "/", "elm", 0, 0) == 0){
           rt_kprintf("File system formatted successfully\n");
       }else{
           if(dfs_mkfs("elm", "spi5") == 0)
               rt_kprintf("File system formatted successfully\n");
       }
       list_device();
*/
