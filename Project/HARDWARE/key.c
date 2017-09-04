#include "key.h"
#include "delay.h"

tKeyStruct key_buf[4];

uint32_t global_key;

//按键初始化函数 
void KEY_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC|
											RCC_APB2Periph_GPIOD,ENABLE);//使能PORTA,PORTC时钟

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//关闭jtag，使能SWD，可以用SWD模式调试
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_8;//PA0 PA8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //设置成上拉输入
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;//PC13
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //设置成上拉输入
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//
 
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;//
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //PA0设置成输入，默认下拉	  
	GPIO_Init(GPIOD, &GPIO_InitStructure);//
	
}

void Get_Key(void)
{
	if(KEY1 == 0)
	{
		if(key_buf[0].key_press_count < FILTER_COUNT)
		{
			key_buf[0].key_press_count++;
		}
		else
		{
			key_buf[0].key_press_flag = 1;
		}
	}
	else
	{
		if(key_buf[0].key_press_count > 0)
		{
			key_buf[0].key_press_count--;
		}
		else
		{
			key_buf[0].key_press_flag = 0;
		}
	}
	
	if(KEY2 == 0)
	{
		if(key_buf[1].key_press_count < FILTER_COUNT)
		{
			key_buf[1].key_press_count++;
		}
		else
		{
			key_buf[1].key_press_flag = 1;
		}
	}
	else
	{
		if(key_buf[1].key_press_count > 0)
		{
			key_buf[1].key_press_count--;
		}
		else
		{
			key_buf[1].key_press_flag = 0;
		}
	}
	
	if(KEY3 == 0)
	{
		if(key_buf[2].key_press_count < FILTER_COUNT)
		{
			key_buf[2].key_press_count++;
		}
		else
		{
			key_buf[2].key_press_flag = 1;
		}
	}
	else
	{
		if(key_buf[2].key_press_count > 0)
		{
			key_buf[2].key_press_count--;
		}
		else
		{
			key_buf[2].key_press_flag = 0;
		}
	}
	
	if(KEY4 == 0)
	{
		if(key_buf[3].key_press_count < FILTER_COUNT)
		{
			key_buf[3].key_press_count++;
		}
		else
		{
			key_buf[3].key_press_flag = 1;
		}
	}
	else
	{
		if(key_buf[3].key_press_count > 0)
		{
			key_buf[3].key_press_count--;
		}
		else
		{
			key_buf[3].key_press_flag = 0;
		}
	}
}


uint32_t Key_Handler(void)
{
	uint32_t key_temp = 0;
	uint8_t i;
	for(i=0;i<4;i++)
	{
		if(key_buf[i].key_press_flag == 1)
		{
			key_buf[i].key_new = 1;
			key_temp |= 1UL<<i;
		}
		else
		{
			key_buf[i].key_new = 0;
			key_temp &= ~(1UL<<i);
		}
	}
	return key_temp;
}

//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//返回值：
//0，没有任何按键按下
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(KEY1==0||KEY2==0||KEY3==0 || KEY4==0))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(KEY1==0)return KEY1_PRES;
		else if(KEY2==0)return KEY2_PRES;
		else if(KEY3==0)return KEY3_PRES; 
		else if(KEY4==0)return KEY4_PRES; 
	}else if(KEY1==1&&KEY2==1&&KEY3==1&&KEY4==1)key_up=1; 	     
	return 0;// 无按键按下
}
