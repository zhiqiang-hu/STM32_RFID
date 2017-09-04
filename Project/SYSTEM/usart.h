#ifndef __USART_H
#define __USART_H

#include "sys.h"
#include "stdio.h"	 

////////////////////////////////////////////////////////////////////////////////// 	
#define USART_RX_MAX_LEN  			512  	//
#define EN_USART1_RX 			1		//
	  	
extern u8  USART_RX_BUF[USART_RX_MAX_LEN]; //
extern u16 USART_RX_STA;         		//
extern u16 USART_BUFF_HEADER;
extern u16 USART_BUFF_TAIL;
 
extern u16 USART_RX_CNT;

void USART1_Init(u32 bound);

#endif	   


















