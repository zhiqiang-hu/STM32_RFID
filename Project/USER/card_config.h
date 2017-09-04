#ifndef __CARD_CONFIG_H
#define __CARD_CONFIG_H

#include "stm32f10x.h"

#define UPDATEADDR1	0x1FE
#define UPDATEADDR2	0x1FF

typedef struct tagCmd
{
	unsigned char rw_con;				//1 BYTE
	unsigned char cmd;					//1 BYTE
	unsigned short addr_tar;		//2 BYTES
	unsigned short addr_src;		//2 BYTES
	unsigned int offset;				//4 BYTES
	unsigned short len;					//2 BYTES
}FPGA_COMMAND;

typedef struct tagMCUUpgradeInfo	// MCU升级数据结构
{
	unsigned long lenth;		//包长度
	unsigned char guid[64];	//唯一码
	unsigned long donetime;	//升级完成时间
	unsigned long reserved;	//保留
	unsigned short jump_times;//跳转次数
	unsigned short crc;			//数据包校验结果             
}MCUUpgradeInfo;

#define version_master 		0x91	 		/*0 : master 	bcd code*/ 
#define version_second 		0x88			/*1 : second 	bcd code*/
#define version_build  		0x88			/*2 : build	 	bcd code*/
#define version_mcu_type 	0x98			/*3 : flag 	 	bcd code*/
#define version_year		  0x15		  /*4 : year 	 	bcd code*/
#define version_month		  0x05			/*5 : month  	bcd code*/
#define version_day			  0x05			/*6 : day 	 	bcd code*/
#define version_hour		  0x15			/*7 : hour 	 	bcd code*/

//带DVI的version_master = 0x89;不带DVI的version_master = 0x90;
//version_mcu_type = 0x98 表示MCU1
//version_mcu_type = 0xA8 表示MCU2

extern const int8_t VERSION[8];
extern const uint8_t DEVICE_SERIAL[];


extern uint8_t buff[512];





























#endif
