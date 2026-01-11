
#ifndef __STM32L476G_sensor_H
#define __STM32L476G_sensor_H
#include "stm32l476xx.h"


#define CNT_PSC	40

void init_sensor();
int get_ticks();

#endif
