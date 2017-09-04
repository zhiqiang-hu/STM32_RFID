#include "timer.h"
#include "key.h"

uint8_t time_tick_5ms = 0;
uint32_t system_time = 0;

void Timer3_Init(uint16_t arr, uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_ClockDivision  = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM3,ENABLE);
}

void Reset_System_Time(void)
{
	time_tick_5ms = 0;
	system_time = 0;
}

uint32_t Get_System_Time(void)
{
	return system_time;
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
		//Add your code here
		//Get_Key();
		time_tick_5ms ++;
		if(time_tick_5ms == 20)
		{
			time_tick_5ms = 0;
			system_time ++;
		}
//		if(buzzer_time > 0)
//		{
//			buzzer_time--;
//			Set_Buzzer();
//		}
//		else
//		{
//			Reset_Buzzer();
//		}
	}
}








































