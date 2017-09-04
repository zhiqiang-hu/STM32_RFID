#include "iap.h"
#include "card_config.h"
#include "spi_flash.h"
#include "crc.h"
#include "string.h"
#include "stm32f10x.h"
#include "stm32f10x_flash.h"

#define FLASH_APP1_ADDR		0x08005000  	//第一个应用程序起始地址(存放在FLASH)
											//保留的空间为IAP使用,预留20KB空间给BOOTLOADER使用

volatile uint8_t iap_buf[1024] = {0}; //用于缓存数据的数组
u16 receiveDataCur = 0;	//当前iapbuffer中已经填充的数据长度,一次填充满了之后写入flash并清零
u32 addrCur = FLASH_APP1_ADDR;			//当前系统写入地址,每次写入之后地址增加2048

typedef  void (*iapfun)(void);				//定义一个函数类型的参数.
iapfun jump2app;

//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

//跳转到应用程序段
//appxaddr:用户代码起始地址.
void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.0x20000000是sram的起始地址,也是程序的栈顶地址
	{ 
		delay_ms(10);
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(vu32*)appxaddr);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		jump2app();									//跳转到APP.
	}
	else
	{
		printf("program in flash is error\r\n");
	}
}

u16 STMFLASH_ReadHalfWord(u32 faddr)
{
    return *(vu16*)faddr; 
}

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
    u16 i;
    for(i=0;i<NumToRead;i++)
    {
        pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.
        ReadAddr+=2;//偏移2个字节.	
    }
}

void JumpToApp(void)
{
	if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
	{	 
		iap_load_app(FLASH_APP1_ADDR);//跳转到app的复位向量地址
	}
	else
	{
		printf("非FLASH应用程序,无法执行!\r\n");
	}
}

uint8_t CheckProgFlag(void)
{
	uint16_t i;
	uint16_t crc_result;
	MCUUpgradeInfo mcu1_data,mcu2_data;
	SPI_Flash_Read(UPDATEADDR1 * 4096,sizeof(MCUUpgradeInfo),(uint8_t *)&mcu1_data);
	SPI_Flash_Read(UPDATEADDR2 * 4096,sizeof(MCUUpgradeInfo),(uint8_t *)&mcu2_data);
	crc_result = make_crc((unsigned char *)&mcu1_data,sizeof(MCUUpgradeInfo)-2);
	if((crc_result != mcu1_data.crc) || (crc_result == 0))
	{
		return 1;
	}
		
	if(0 == memcmp(&mcu1_data,&mcu2_data,sizeof(MCUUpgradeInfo) ))
	{
		return 0;
	}
		
	if ( mcu1_data.jump_times > 0 )
	{
		mcu1_data.jump_times--;
		mcu1_data.crc = make_crc((unsigned char *)&mcu1_data,sizeof(MCUUpgradeInfo)-2);
		SPI_FLASH_Erase(UPDATEADDR1,UPDATEADDR1);
		memcpy(buff,(uint8_t *)&mcu1_data,sizeof(MCUUpgradeInfo));
		SPI_Flash_Write(UPDATEADDR1*4096,256,buff);
		return 0;
	}
	return 1;
}



uint8_t EraseMcuFlash(uint8_t bgn,uint8_t end)
{
	uint8_t i;
	FLASH_Status flash_status = FLASH_COMPLETE;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	for(i=bgn;i<=end;i++)
	{
		flash_status = FLASH_ErasePage(i*FLASH_PAGE_SIZE + 0x08000000);
		if(flash_status != FLASH_COMPLETE)
		{
			return 1;
		}
	}
	return 0;
}



//WriteAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数   
uint8_t STMFLASH_Write_AndCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
    u16 i;
		u32 tmp_addr;
    FLASH_Status flash_status = FLASH_COMPLETE;
		tmp_addr = WriteAddr;
    for(i=0;i<NumToWrite;i++)
    {
        flash_status = FLASH_ProgramHalfWord(tmp_addr,pBuffer[i]);
        if(flash_status != FLASH_COMPLETE)
        {
					return 1;
        }
        tmp_addr+=2;//地址增加2.
    }
		tmp_addr = WriteAddr;
		for(i=0;i<NumToWrite;i++)
    {
				if(pBuffer[i] !=STMFLASH_ReadHalfWord(tmp_addr))//读取2个字节.
				{
					return 1;
				}
        tmp_addr+=2;//偏移2个字节.	
    }
    return 0;
} 


uint8_t WriteMcuFlash(uint8_t ben,uint8_t end,uint32_t des)
{
  if(des<STM32_FLASH_BASE||(des>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))
	{
		return 2;//非法地址
	}
    FLASH_Unlock();						//解锁
    if(STMFLASH_Write_AndCheck(des,(u16*)iap_buf,STM_SECTOR_SIZE/2))//写已经擦除了的,直接写入扇区剩余区间.
    {
			FLASH_Lock();//上锁
			return 1;
    }
    FLASH_Lock();//上锁
    return 0;
}

