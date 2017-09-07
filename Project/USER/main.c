
/* Includes ------------------------------------------------------------------*/
#include "spi_flash.h"
#include "delay.h"
#include "usart.h"
#include "usbio.h"
#include "led.h"
#include "key.h"
#include "iap.h"
#include "card_config.h"
#include "spi_flash.h"
#include "timer.h"
#include "wdg.h"
#include "lcd5110.h"
#include "rc522.h"

//#include <stdio.h>
#include <string.h>
//#include <ctype.h>
//#include <stdlib.h>
//#include <stdarg.h>


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/*******************************************************************************

					注意:大容量STM32单片机内部FLASH扇区大小为2048字节




*********************************************************************************/

FPGA_COMMAND FPGA;
FPGA_COMMAND COM_BAK;
uint8_t usb_sta;
const u8 RFID_PASSWORD_A[]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

void TaskRead(void);
void TaskWriteAck(void);



int main(void)
{
	u16 i;
	volatile u8 read_sta=0;
	u8 RFID_ID_CODE[4];
	u8 RFID_DATA_BUF0[16] = {0};
	delay_init(72);
	JTAG_Set(SWD_ENABLE);
	USART1_Init(115200);
	LED_Init();
//	SPI_Flash_Init();
	RCC522_GPIO_Init();
	RCC522_Init();
	LCD_GPIO_Init();
	LCD5110_Init();
	
	Timer3_Init(49,7199);//7200分频，72000000/7200=10000  50 = 5ms
//	if((0 == CheckProgFlag()))
//	{
//		JumpToApp();
//	}
//	else
//	{
//		printf("No Application Code!\n");
//	}
	Set_System();//系统时钟初始化
	USB_Interrupts_Config();
	Set_USBClock();
	USB_Init();
	for(i=0;i<16;i++)
	{
		RFID_DATA_BUF0[i]=i;
	}
//	Write_Block(2,RFID_DATA_BUF0,(uint8_t *)RFID_PASSWORD_A);
//	IWDG_Init(4,625);//开启看门狗，定时时间为：1s
	memset(RFID_DATA_BUF0,0,16);
	while(1)
	{
//		LCD_Clear();
		RFID_Get_ID(RFID_ID_CODE);
		read_sta = Read_Block(2,RFID_DATA_BUF0,(uint8_t *)RFID_PASSWORD_A);
		delay_us(50);
		//LCD_Write_EnStr(0,0,"hello");
		LCD_Printf(0,2,"id=%08X",*((unsigned int*)RFID_ID_CODE));
		LCD_Printf(0,3,"count=%d",i++);
		memset(RFID_ID_CODE,0,4);
		memset(RFID_DATA_BUF0,0,16);
		LED1 = !LED1;
		delay_ms(500);
//		IWDG_Feed();
		FPGA.rw_con = 0xff;
		usb_sta = ReadPort1(sizeof(FPGA_COMMAND),(uint8_t *)&FPGA);
		if(usb_sta == 0)
		{
			COM_BAK = FPGA;
			printf("rw_con = %02X\ncmd = %02X\naddr_tar = %04X\naddr_src = %04X\noffset = %08X\n len = %04X\n",
			COM_BAK.rw_con,COM_BAK.cmd,COM_BAK.addr_tar,COM_BAK.addr_src,COM_BAK.offset,COM_BAK.len);
			switch(COM_BAK.rw_con)
			{
				case 0x00:
				{
					TaskRead();
				}break;
				
				case 0x02:
				{
					TaskWriteAck();
				}break;
			}
		}
	}
}

void TaskRead(void)
{
	uint8_t ack = 0x01;
	switch(COM_BAK.cmd)
	{
		
		case 0x5A:
  	{
 		  usb_sta = WritePort1(1,&ack);	//
//			faddr[0]=COM_BAK.offset & 0xff;
//	   	faddr[1]=(COM_BAK.offset>>8) & 0xff;
//   		faddr[2]=(COM_BAK.offset>>16) & 0xff;
//   		faddr[3]=(COM_BAK.offset>>24) & 0xff;
//	    flen[0]=COM_BAK.len & 0xff;
//	    flen[1]=(COM_BAK.len>>8) & 0xff;
    	buff[1] = COM_BAK.len & 0xff;;	    //
			buff[0] = (COM_BAK.len>>8) & 0xff;	//
			if(COM_BAK.offset==0x640000)
    	{
//    		if(DVI_STA==0x55)
 //   		{
//    			buff_DSE[0] = 0xdd;	//
//					buff_DSE[1] = 0xdd;	//
//				}
			}
			WritePort2((COM_BAK.len+2),buff);	//
  	}break;
		
		case 0x1D://读取SPI FLASH的内容
		{
			WritePort1(1,&ack);//应答主机
			SPI_Flash_Read(COM_BAK.offset,COM_BAK.len,&buff[2]);
			buff[0] = 1;	//
			buff[1] = 0;	//
			WritePort2((COM_BAK.len+2),buff);
		}break;
		
		case 0xAC://握手命令
		{
			WritePort1(1,&ack);
			buff[0] = 0xac;
			buff[1] = 8;
			buff[2]=0x08;
			buff[3]=0x07;
			buff[4]=0x06;
			buff[5]=0x05;
			buff[6]=0x04;
			buff[7]=0x03;
			buff[8]=0x02;
			buff[9]=0x01;
			WritePort2(10,buff);
		}break;
		case 0x8a: // MCU版本信息
		{
			WritePort1(1,&ack);	//
			buff[0] = 0x8a;	//
			buff[1] = 8;	//
			memcpy(&buff[2],VERSION,8);
			WritePort2(10,buff);	//
		}
		break;
		case 0xAD: // MCU Device Code D2BEF08016H
		{
			WritePort1(1,&ack);
			buff[0] = 0xAD;	//
			buff[1] = 0xA5;	//
//			for(i=0; i<=15; i++)
//			{
//				buff[i+2] = DEVICE_SERIAL[i];//测试用的DEVICE CODE
//			}
			SPI_Flash_Read(0x7f000,16,&buff[2]);
			WritePort2(18,buff);
		}
		break;
		
		case 0xAE:// handshake command2
		{
			WritePort1(1,&ack);
			buff[0] = 0xae;	//
			buff[1] = 16;	//
			buff[2]=0x0a;
			buff[3]=0x07;	//Hardware Type
			buff[4]=0x05;	//USB Interface
			buff[5]=0x05; //
			buff[6]=0x00;	
			buff[7]=0x01; //Version
			buff[6]=0x00;	
			buff[7]=0x01;  //Reserved
			WritePort2(18,buff);	//
		}
		break; 
	default:
		{
			ack = 0x30; // no cmd
			WritePort1(1,&ack);	//
		}
		break;
	}
}


//
void TaskWriteAck(void)
{
	uint8_t temp;
	uint8_t ack = 0x02;
	uint8_t cmd;
	uint8_t error;
	cmd=  COM_BAK.cmd;
	switch(cmd)
	{	
		case 0x1D:	//写入MCU升级标志
		{
			WritePort1(1,&ack);				
			usb_sta = ReadPort2(COM_BAK.len,&buff[2]);
			SPI_Flash_Write(COM_BAK.offset,256,&buff[2]);
			buff[0] = 1;
			buff[1] = 0;
			WritePort1(2,&buff[0]);
		}
		break;
		
		case 0x2E: //擦除升级标志
		{
			temp = 0;
			if( COM_BAK.offset>=0x80)
			{
				SPI_FLASH_Erase(COM_BAK.offset,COM_BAK.offset);
				temp=1;
			}
			else
			{
				if (COM_BAK.addr_tar == 0xa598 && COM_BAK.addr_src == 0x98a5)
				{
					SPI_FLASH_Erase(COM_BAK.offset,COM_BAK.offset);
					temp = 1;
				}
			}
			WritePort1(1,&temp);
		}
		break;	
		
		case 0xf0: //擦除MCU1内部FLASH
		{
			error = EraseMcuFlash(COM_BAK.addr_tar,COM_BAK.addr_src);		//
			WritePort1(1,&error);
		}
		break;
		
		case 0xf1: //MCU1 FLASH
		{
			WritePort1(1,&ack);		
			usb_sta = ReadPort2(COM_BAK.len,(uint8_t*)iap_buf);	//		
			error = WriteMcuFlash(COM_BAK.addr_tar,COM_BAK.addr_src,COM_BAK.offset);
			WritePort1(1,&error);			//
		}
		break;
		case 0xff://跳转到新程序
		{
			delay_ms(1000);
			delay_ms(1000);
			delay_ms(1000);
		}
		break;
	default :
		break;
	}
}

//--------------------------------------------------------------------------------------------
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
