#ifndef __IAP_H_
#define __IAP_H_

#include "usart.h"
#include "sys.h"
#include "delay.h"


//0x08000000 flash起始地址
//0x2000 iap代码长度
//0x1ffc-0x1fff作为固化配置存在

extern volatile uint8_t iap_buf[]; //用于缓存数据的数组


#define STM32_FLASH_SIZE 128 	 		//所选STM32的FLASH容量大小(单位为K)
#define FLASH_PAGE_SIZE (1024)
#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //字节
#else 
#define STM_SECTOR_SIZE	2048
#endif

//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址


uint8_t CheckProgFlag(void);
void JumpToApp(void);
uint8_t EraseMcuFlash(uint8_t bgn,uint8_t end);
uint8_t WriteMcuFlash(uint8_t ben,uint8_t end,uint32_t des);
#endif


















