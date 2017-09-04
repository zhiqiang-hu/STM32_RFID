#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x_tim.h"

extern uint32_t system_time;

void Timer3_Init(uint16_t arr, uint16_t psc);
void Reset_System_Time(void);
uint32_t Get_System_Time(void);


































#endif

