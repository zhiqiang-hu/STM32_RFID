#ifndef __LCD5110_H
#define __LCD5110_H

#include "sys.h"

/**************************枚举 D/C模式选择 **************************/
typedef enum
{
  DC_CMD  = 0,	//写命令
	DC_DATA = 1		//写数据	
} DCType;


#define LCD_DC		PBout(12)
#define LCD_RST		PBout(3)
#define LCD_CE		PAout(15)

#define EN_SPI2_DMA 0




void LCD_GPIO_Init(void);
void LCD5110_Init(void);
void LCD_Send(u8 data, DCType dc);
void LCD_Clear(void);
void LCD_Write_EnStr(u8 X, u8 Y, u8* s);
void LCD_Printf(u8 x,u8 y,const char *fmt,...);







#endif


