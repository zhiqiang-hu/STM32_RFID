#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

#define FILTER_COUNT 3
#define KEY_LONG_NUM 2000

typedef struct _keyStrut
{
	uint16_t key_press_count;
	uint8_t key_press_flag;
	uint8_t key_press_long;
	uint8_t key_new;
	uint8_t key_old;
}tKeyStruct; 

#define KEY_NUM1 (1UL << 0)
#define KEY_NUM2 (1UL << 1)
#define KEY_NUM3 (1UL << 2)
#define KEY_NUM4 (1UL << 3)


#define KEY1  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//读取按键0
#define KEY2  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)//读取按键1
#define KEY3  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)//读取按键2 
#define KEY4  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_3)//读取按键2 

#define KEY1_PRES	1		// 
#define KEY2_PRES	2		//
#define KEY3_PRES	3		// 
#define KEY4_PRES	4		// 

extern uint32_t global_key;
extern tKeyStruct key_buf[4];

void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8 mode);  	//按键扫描函数				
void Get_Key(void);
uint32_t Key_Handler(void);
#endif
