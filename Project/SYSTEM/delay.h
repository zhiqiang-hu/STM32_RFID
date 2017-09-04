#ifndef __DELAY_H
#define __DELAY_H 			   

#include "stm32f10x.h"



extern RCC_ClocksTypeDef  rcc_clocks;  	 //在tzsk.c中定义的芯片时钟频率结构变量

void delay_init(u8 SYSCLK);
void delay_us(u32 nus);
void delay_ms(u16 nms);
#endif



