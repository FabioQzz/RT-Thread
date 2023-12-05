/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-12-01     39346       the first version
 */

#include "sensing.h"
#include <stdlib.h>

int read_sensor_sx(void){
    return random_values();
}
int read_sensor_dx(void){
    return random_values();
}

int read_heart_rate(){
    int min = 50;
    int max = 120;

    int min1 = 60;
    int max1 = 80;

    if(flag_sens == 0){
        return min + rand() % (max+1 - min);
    }else{
        if(flag_sens == 1)
            return min1 + rand() % (max1+1 - min1);
    }
    return 0;
}


/*This function emulates the sensing of electrical potential values by generating random values in the intervals [min,max] and [min1,max1]
 * NB: the selection of the right interval depends on the current value of the flag. This flag is a global variable set by ThreadD,
 *     when it is activated that reports the electrical potential values (​​of the two atrial cells) at correct intervals  */
int random_values(void)
{
    int min = 0;    //Interval where threadD can be activated
    int max = 100;

    int min1 = 45 ; //Interval where threadD cannot be activated since it was just executed
    int max1 = 65 ;

    int value;
    static int time = 0;   //This counter has the purpose to force the flag back to 0 so that threadD can again be executed

    while (!flag_sens) {
        return value = min + rand() % (max+1 - min);     //Sensing part: generate a random value [0-100]
    }
    while (flag_sens) {
        time ++;

        if(time>100){
                flag_sens=0;
        }
        return value = min1 + rand() % (max1+1 - min1);  //Sensing part: generate a random value [45-65]
    }

    return 0;
}
