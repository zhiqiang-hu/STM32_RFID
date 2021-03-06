#include "lcd5110.h"
#include "spi.h"
#include "delay.h"
#include "stdio.h"
#include "stdarg.h"

static u8 lcd_buf[84*6];

const unsigned char  Font6x8[][6] =
{
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // sp
	{ 0x00, 0x00, 0x00, 0x2f, 0x00, 0x00 },   // !
	{ 0x00, 0x00, 0x07, 0x00, 0x07, 0x00 },   // "
	{ 0x00, 0x14, 0x7f, 0x14, 0x7f, 0x14 },   // #
	{ 0x00, 0x24, 0x2a, 0x7f, 0x2a, 0x12 },   // $
	{ 0x00, 0x62, 0x64, 0x08, 0x13, 0x23 },   // %
	{ 0x00, 0x36, 0x49, 0x55, 0x22, 0x50 },   // &
	{ 0x00, 0x00, 0x05, 0x03, 0x00, 0x00 },   // '
	{ 0x00, 0x00, 0x1c, 0x22, 0x41, 0x00 },   // (
	{ 0x00, 0x00, 0x41, 0x22, 0x1c, 0x00 },   // )
	{ 0x00, 0x14, 0x08, 0x3E, 0x08, 0x14 },   // *
	{ 0x00, 0x08, 0x08, 0x3E, 0x08, 0x08 },   // +
	{ 0x00, 0x00, 0x00, 0xA0, 0x60, 0x00 },   // ,
	{ 0x00, 0x08, 0x08, 0x08, 0x08, 0x08 },   // -
	{ 0x00, 0x00, 0x60, 0x60, 0x00, 0x00 },   // .
	{ 0x00, 0x20, 0x10, 0x08, 0x04, 0x02 },   // /
	{ 0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E },   // 0
	{ 0x00, 0x00, 0x42, 0x7F, 0x40, 0x00 },   // 1
	{ 0x00, 0x42, 0x61, 0x51, 0x49, 0x46 },   // 2
	{ 0x00, 0x21, 0x41, 0x45, 0x4B, 0x31 },   // 3
	{ 0x00, 0x18, 0x14, 0x12, 0x7F, 0x10 },   // 4
	{ 0x00, 0x27, 0x45, 0x45, 0x45, 0x39 },   // 5
	{ 0x00, 0x3C, 0x4A, 0x49, 0x49, 0x30 },   // 6
	{ 0x00, 0x01, 0x71, 0x09, 0x05, 0x03 },   // 7
	{ 0x00, 0x36, 0x49, 0x49, 0x49, 0x36 },   // 8
	{ 0x00, 0x06, 0x49, 0x49, 0x29, 0x1E },   // 9
	{ 0x00, 0x00, 0x36, 0x36, 0x00, 0x00 },   // :
	{ 0x00, 0x00, 0x56, 0x36, 0x00, 0x00 },   // ;
	{ 0x00, 0x08, 0x14, 0x22, 0x41, 0x00 },   // <
	{ 0x00, 0x14, 0x14, 0x14, 0x14, 0x14 },   // =
	{ 0x00, 0x00, 0x41, 0x22, 0x14, 0x08 },   // >
	{ 0x00, 0x02, 0x01, 0x51, 0x09, 0x06 },   // ?
	{ 0x00, 0x32, 0x49, 0x59, 0x51, 0x3E },   // @
	{ 0x00, 0x7C, 0x12, 0x11, 0x12, 0x7C },   // A
	{ 0x00, 0x7F, 0x49, 0x49, 0x49, 0x36 },   // B
	{ 0x00, 0x3E, 0x41, 0x41, 0x41, 0x22 },   // C
	{ 0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C },   // D
	{ 0x00, 0x7F, 0x49, 0x49, 0x49, 0x41 },   // E
	{ 0x00, 0x7F, 0x09, 0x09, 0x09, 0x01 },   // F
	{ 0x00, 0x3E, 0x41, 0x49, 0x49, 0x7A },   // G
	{ 0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F },   // H
	{ 0x00, 0x00, 0x41, 0x7F, 0x41, 0x00 },   // I
	{ 0x00, 0x20, 0x40, 0x41, 0x3F, 0x01 },   // J
	{ 0x00, 0x7F, 0x08, 0x14, 0x22, 0x41 },   // K
	{ 0x00, 0x7F, 0x40, 0x40, 0x40, 0x40 },   // L
	{ 0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F },   // M
	{ 0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F },   // N
	{ 0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E },   // O
	{ 0x00, 0x7F, 0x09, 0x09, 0x09, 0x06 },   // P
	{ 0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E },   // Q
	{ 0x00, 0x7F, 0x09, 0x19, 0x29, 0x46 },   // R
	{ 0x00, 0x46, 0x49, 0x49, 0x49, 0x31 },   // S
	{ 0x00, 0x01, 0x01, 0x7F, 0x01, 0x01 },   // T
	{ 0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F },   // U
	{ 0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F },   // V
	{ 0x00, 0x3F, 0x40, 0x38, 0x40, 0x3F },   // W
	{ 0x00, 0x63, 0x14, 0x08, 0x14, 0x63 },   // X
	{ 0x00, 0x07, 0x08, 0x70, 0x08, 0x07 },   // Y
	{ 0x00, 0x61, 0x51, 0x49, 0x45, 0x43 },   // Z
	{ 0x00, 0x00, 0x7F, 0x41, 0x41, 0x00 },   // [
	{ 0x00, 0x55, 0x2A, 0x55, 0x2A, 0x55 },   // 55
	{ 0x00, 0x00, 0x41, 0x41, 0x7F, 0x00 },   // ]
	{ 0x00, 0x04, 0x02, 0x01, 0x02, 0x04 },   // ^
	{ 0x00, 0x40, 0x40, 0x40, 0x40, 0x40 },   // _
	{ 0x00, 0x00, 0x01, 0x02, 0x04, 0x00 },   // '
	{ 0x00, 0x20, 0x54, 0x54, 0x54, 0x78 },   // a
	{ 0x00, 0x7F, 0x48, 0x44, 0x44, 0x38 },   // b
	{ 0x00, 0x38, 0x44, 0x44, 0x44, 0x20 },   // c
	{ 0x00, 0x38, 0x44, 0x44, 0x48, 0x7F },   // d
	{ 0x00, 0x38, 0x54, 0x54, 0x54, 0x18 },   // e
	{ 0x00, 0x08, 0x7E, 0x09, 0x01, 0x02 },   // f
	{ 0x00, 0x18, 0xA4, 0xA4, 0xA4, 0x7C },   // g
	{ 0x00, 0x7F, 0x08, 0x04, 0x04, 0x78 },   // h
	{ 0x00, 0x00, 0x44, 0x7D, 0x40, 0x00 },   // i
	{ 0x00, 0x40, 0x80, 0x84, 0x7D, 0x00 },   // j
	{ 0x00, 0x7F, 0x10, 0x28, 0x44, 0x00 },   // k
	{ 0x00, 0x00, 0x41, 0x7F, 0x40, 0x00 },   // l
	{ 0x00, 0x7C, 0x04, 0x18, 0x04, 0x78 },   // m
	{ 0x00, 0x7C, 0x08, 0x04, 0x04, 0x78 },   // n
	{ 0x00, 0x38, 0x44, 0x44, 0x44, 0x38 },   // o
	{ 0x00, 0xFC, 0x24, 0x24, 0x24, 0x18 },   // p
	{ 0x00, 0x18, 0x24, 0x24, 0x18, 0xFC },   // q
	{ 0x00, 0x7C, 0x08, 0x04, 0x04, 0x08 },   // r
	{ 0x00, 0x48, 0x54, 0x54, 0x54, 0x20 },   // s
	{ 0x00, 0x04, 0x3F, 0x44, 0x40, 0x20 },   // t
	{ 0x00, 0x3C, 0x40, 0x40, 0x20, 0x7C },   // u
	{ 0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C },   // v
	{ 0x00, 0x3C, 0x40, 0x30, 0x40, 0x3C },   // w
	{ 0x00, 0x44, 0x28, 0x10, 0x28, 0x44 },   // x
	{ 0x00, 0x1C, 0xA0, 0xA0, 0xA0, 0x7C },   // y
	{ 0x00, 0x44, 0x64, 0x54, 0x4C, 0x44 },   // z
	{ 0x14, 0x14, 0x14, 0x14, 0x14, 0x14 }    // horiz lines
};

#if EN_SPI2_DMA
DMA_InitTypeDef DMA_initstructure ;
void DMA_SPI_Config(void)
{  
		
   // 初始化DMA 结构体
    DMA_initstructure.DMA_BufferSize = 0 ;
    DMA_initstructure.DMA_DIR = DMA_DIR_PeripheralDST ;
    DMA_initstructure.DMA_M2M = DISABLE ;
    DMA_initstructure.DMA_MemoryBaseAddr = (u32)lcd_buf;
    DMA_initstructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte ;
    DMA_initstructure.DMA_MemoryInc = DMA_MemoryInc_Enable ;
    DMA_initstructure.DMA_Mode = DMA_Mode_Normal;
    //spi2 data rigester address 
    DMA_initstructure.DMA_PeripheralBaseAddr = (u32)&SPI2->DR ;
    DMA_initstructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte ;
    DMA_initstructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_initstructure.DMA_Priority = DMA_Priority_High ;  
}
#endif

void LCD_GPIO_Init(void)
{
	//LCD_DC			--------> PB12
	//LCD_RST			--------> PB3
	//LCD_CE			--------> PA15
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_3;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	LCD_CE = 1;
}

void LCD5110_Init(void)
{
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
#if EN_SPI2_DMA
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);
#endif
	SPI2_Init();
#if EN_SPI2_DMA
	DMA_SPI_Config();
#endif
	LCD_RST = 0;
	delay_ms(1);
	LCD_RST = 1;
	LCD_CE = 1;
	delay_ms(1);
	LCD_CE = 0;
	//设置LCD
	LCD_Send(0x21, DC_CMD);	//使用扩展命令设置LCD模式
	LCD_Send(0xC8, DC_CMD);	//设置偏置电压
	LCD_Send(0x06, DC_CMD);	//温度校正
	LCD_Send(0x13, DC_CMD); //1:48
	LCD_Send(0x20, DC_CMD);	//使用基本命令
	
	LCD_Send(0x0C, DC_CMD); //设定显示模式，普通显示
	//LCD_Send(0x0D, DC_CMD);	//设定显示模式，反转显示
	LCD_Clear();
	LCD_CE = 1;
}

/*******************************************************************************
* Function Name  : LCD_Send
* Description    : 向LCD发送数据
* Input          : u8 data, DCType dc
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Send(u8 data, DCType dc)
{
//	LCD_CE = 0;
//  while( SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE ) == RESET );
	if (dc==DC_CMD)
		LCD_DC = 0;	//发送命令
	else
		LCD_DC = 1;//发送数据
	SPI2_ReadWriteByte(data);
//    SPI_I2S_SendData(SPI2, data);
//    //等待数据完成，否则LCD_SET_XY 会有问题
//    while( SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE ) == RESET );
//	LCD_CE = 1;
}

/*******************************************************************************
* Function Name  : LCD_SetContrast
* Description    : 设置LCD对比度(对比度范围: 0~127)
* Input          : u8 contrast
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_SetContrast(u8 contrast)
{
	LCD_CE = 0;
    LCD_Send(0x21, DC_CMD);
    LCD_Send(0x80 | contrast, DC_CMD);
    LCD_Send(0x20, DC_CMD);
	LCD_CE = 1;
}
/*******************************************************************************
* Function Name  : LCD_SetXY
* Description    : 设置LCD当前坐标
* Input          : u8 X, u8 Y	
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_SetXY(u8 X, u8 Y)
{
	if (X>13) X = 13;
	if (Y>5) Y = 5;
	X *= 6;
	LCD_Send(0x80 | X, DC_CMD);    // 列 
	delay_ms(10);  
	LCD_Send(0x40 | Y, DC_CMD);    // 行
}
/*******************************************************************************
* Function Name  : LCD_Clear
* Description    : LCD清屏
* Input          : None	
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Clear(void)
{
	u16 i;
	LCD_CE = 0;
  LCD_Send(0x80, DC_CMD);
	delay_ms(1);
	LCD_Send(0x40, DC_CMD);
	for(i=0; i<504; i++)
	LCD_Send(0, DC_DATA);	  
	LCD_CE = 1;
}

/*******************************************************************************
* Function Name  : LCD_Write_Char
* Description    : 向LCD写一个英文字符
* Input          : u8 ascii
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Write_Char(u8 ascii)
{
    u8 n;
	LCD_CE = 0;
    ascii -= 32; //ASCII码-32
    for (n=0; n<6; n++)
		LCD_Send(Font6x8[ascii][n], DC_DATA);
	LCD_CE = 1;
}

/*******************************************************************************
* Function Name  : LCD_Write_EnStr
* Description    : 向LCD写英文字符串
* Input          : u8 ascii
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Write_EnStr(u8 X, u8 Y, u8* s)
{
	
    u8 * str = s;
    int i = 0;
    int lcd_index =0;
    if(str == 0 )
    {
        return ;
    }
		LCD_CE = 0;
#if EN_SPI2_DMA
    //等待上次DMA请求结束
    while(DMA_GetCurrDataCounter(DMA1_Channel5)) ;  
#endif
    LCD_SetXY(X,Y);
    while(*str)
    {
        //拷贝显示数据到缓冲区
        for(i=0;i<6;i++)
        {
            lcd_buf[lcd_index ++ ] = Font6x8[*str - 32][i];
        }
					//memcpy(lcd_buf+6*str,&Font6x8[*str - 32][0],6);	
        str ++ ;
    }
    lcd_buf[lcd_index ++ ] = 0 ; // lcd_index ++ 多发送一个0否者最后一个字符会缺少一个像素
#if EN_SPI2_DMA
    DMA_initstructure.DMA_BufferSize = lcd_index ;
   
    DMA_Cmd(DMA1_Channel5, DISABLE); 
    DMA_Init( DMA1_Channel5, &DMA_initstructure) ;
    DMA_Cmd(DMA1_Channel5, ENABLE);
    LCD_DC = 1;  
    SPI_I2S_DMACmd( SPI2, SPI_I2S_DMAReq_Tx, ENABLE) ;
    #else
    for(i = 0 ;i<lcd_index ;i++)
    {
        LCD_Send(lcd_buf[i], DC_DATA);
    }
    #endif
		LCD_CE = 1;
}

void LCD_Printf(u8 x,u8 y,const char *fmt,...)
{
		va_list ap;  
		char string[1024];//
		va_start(ap,fmt);  
		vsprintf(string,fmt,ap); 
		LCD_Write_EnStr(x,y,(u8*)string);
		va_end(ap);  
}
