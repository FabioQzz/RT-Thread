/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-12-01     39346       the first version
 */
#ifndef APPLICATIONS_SENSING_H_
#define APPLICATIONS_SENSING_H_

int read_sensor_sx(void);
int read_sensor_dx(void);
int read_heart_rate();
int random_values(void);

static int flag_sens = 0;


#endif /* APPLICATIONS_SENSING_H_ */
