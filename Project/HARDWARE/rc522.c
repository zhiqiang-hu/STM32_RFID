#include "rc522.h"
#include "led.h"
#include "spi.h"
#include "string.h"

uint8_t RFID_DATA_BUF[32],MLastSelectedSnr[4];
u8 XM[16];
uint16_t RFID_Block_Num;
void delay_ns(u32 ns)
{
  u32 i;
  for(i=0;i<ns;i++)
  {
    __nop();
    __nop();
    __nop();
  }
}

/////////////////////////////////////////////////////////////////////
//功    能：读RC632寄存器
//参数说明：Address[IN]:寄存器地址
//返    回：读出的值
/////////////////////////////////////////////////////////////////////
u8 ReadRawRC(u8   Address)
{
    u8   ucAddr;
    u8   ucResult=0;
		CLR_SPI_CS;
    ucAddr = ((Address<<1)&0x7E)|0x80;
		SPI2_ReadWriteByte(ucAddr);
		ucResult=SPI2_ReadWriteByte(0xff);
		SET_SPI_CS;
    return ucResult;
}

/////////////////////////////////////////////////////////////////////
//功    能：写RC632寄存器
//参数说明：Address[IN]:寄存器地址
//          value[IN]:写入的值
/////////////////////////////////////////////////////////////////////
void WriteRawRC(u8   Address, u8   value)
{  
    u8   ucAddr;
	
		CLR_SPI_CS;
    ucAddr = ((Address<<1)&0x7E);
		SPI2_ReadWriteByte(ucAddr);
		SPI2_ReadWriteByte(value);
		SET_SPI_CS;
}

/////////////////////////////////////////////////////////////////////
//功    能：置RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:置位值
/////////////////////////////////////////////////////////////////////
void SetBitMask(u8   reg,u8   mask)  
{
    char   tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//功    能：清RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:清位值
/////////////////////////////////////////////////////////////////////
void ClearBitMask(u8   reg,u8   mask)  
{
    char   tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
}

char PcdReset(void)
{
	SET_RC522RST;
	delay_ns(10);
	CLR_RC522RST;
	delay_ns(10);
	SET_RC522RST;
	delay_ns(10);
	WriteRawRC(CommandReg,PCD_RESETPHASE);
//	WriteRawRC(CommandReg,PCD_RESETPHASE);
	delay_ns(10);
	WriteRawRC(ModeReg,0x3D);            //和Mifare卡通讯，CRC初始值0x6363
	WriteRawRC(TReloadRegL,30);           
	WriteRawRC(TReloadRegH,0);
	WriteRawRC(TModeReg,0x8D);
	WriteRawRC(TPrescalerReg,0x3E);
	WriteRawRC(TxAutoReg,0x40);//必须要
	return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//关闭天线
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff(void)
{
	ClearBitMask(TxControlReg, 0x03);
}

/////////////////////////////////////////////////////////////////////
//开启天线  
//每次启动或关闭天线发射之间应至少有1ms的间隔
/////////////////////////////////////////////////////////////////////
void PcdAntennaOn(void)
{
    u8   i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}

void Reset_RC522(void)
{
	PcdReset();       //功    能：复位RC522
	PcdAntennaOff();  //关闭天线
	PcdAntennaOn();   //开启天线
}

//////////////////////////////////////////////////////////////////////
//设置RC632的工作方式 
//////////////////////////////////////////////////////////////////////
char M500PcdConfigISOType(u8   type)
{
   if (type == 'A')                     //ISO14443_A
   { 
			ClearBitMask(Status2Reg,0x08);
			WriteRawRC(ModeReg,0x3D);//3F
			WriteRawRC(RxSelReg,0x86);//84
			WriteRawRC(RFCfgReg,0x7F);   //4F
			WriteRawRC(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
			WriteRawRC(TReloadRegH,0);
			WriteRawRC(TModeReg,0x8D);
			WriteRawRC(TPrescalerReg,0x3E);
			delay_ns(1000);
			PcdAntennaOn();
   }
   else{ return 1; }
   
   return MI_OK;
}

void RCC522_GPIO_Init(void)
{
	GPIOB->CRL&=0XFF0FFFFF; 	 
	GPIOB->CRL|=0X00300000; 
	GPIOB->CRL&=0XFFFFFF0F; 	 
	GPIOB->CRL|=0X00000030; 

	GPIOB->CRH&=0X000FFFFF; 	 //PB.13 PB.14 PB.15
	GPIOB->CRH|=0XB8B00000;
	RFID_CS = 1;

}

void RCC522_Init(void)
{
	SPI2_Init();
	Reset_RC522();
	M500PcdConfigISOType( 'A' );
}

/////////////////////////////////////////////////////////////////////
//功    能：寻卡
//参数说明: req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//          pTagType[OUT]：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(u8 req_code,u8 *pTagType)
{
	char   status;  
	u8   unLen;
	u8   ucComMF522Buf[MAXRLEN]; 
	
	ClearBitMask(Status2Reg,0x08);
	WriteRawRC(BitFramingReg,0x07);
	SetBitMask(TxControlReg,0x03);
 
	ucComMF522Buf[0] = req_code;

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);

	if ((status == MI_OK) && (unLen == 0x10))
	{    
		*pTagType     = ucComMF522Buf[0];
		*(pTagType+1) = ucComMF522Buf[1];
	}
	else
	{   status = MI_ERR;   }
   
	return status;
}


/////////////////////////////////////////////////////////////////////
//功    能：防冲撞
//参数说明: pSnr[OUT]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////  
char PcdAnticoll(u8 *pSnr)
{
    char   status;
    u8   i,snr_check=0;
    u8   unLen;
    u8   ucComMF522Buf[MAXRLEN]; 
    

    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(BitFramingReg,0x00);
    ClearBitMask(CollReg,0x80);
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)
    {
    	 for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
         }
         if (snr_check != ucComMF522Buf[i])
         {   status = MI_ERR;    }
    }
    
    SetBitMask(CollReg,0x80);
    return status;
}


/////////////////////////////////////////////////////////////////////
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect(u8 *pSnr)
{
    char   status;
    u8   i;
    u8   unLen;
    u8   ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
  
    ClearBitMask(Status2Reg,0x08);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
    
    if ((status == MI_OK) && (unLen == 0x18))
    {   status = MI_OK;  }
    else
    {   status = MI_ERR;    }

    return status;
}


/////////////////////////////////////////////////////////////////////
//功    能：验证卡片密码
//参数说明: auth_mode[IN]: 密码验证模式
//                 0x60 = 验证A密钥
//                 0x61 = 验证B密钥 
//          addr[IN]：块地址
//          pKey[IN]：密码
//          pSnr[IN]：卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////               
char PcdAuthState(u8   auth_mode,u8   addr,u8 *pKey,u8 *pSnr)
{
    char   status;
    u8   unLen;
//     u8   i;
		u8	 ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
//    for (i=0; i<6; i++)
//    {    ucComMF522Buf[i+2] = *(pKey+i);   }
//    for (i=0; i<6; i++)
//    {    ucComMF522Buf[i+8] = *(pSnr+i);   }
    memcpy(&ucComMF522Buf[2], pKey, 6); 
    memcpy(&ucComMF522Buf[8], pSnr, 4); 
    
    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
    {   status = MI_ERR;   }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：读取M1卡一块数据
//参数说明: addr[IN]：块地址
//          p [OUT]：读出的数据，16字节
//返    回: 成功返回MI_OK
///////////////////////////////////////////////////////////////////// 
char PcdRead(u8   addr,u8 *p )
{
    char   status;
    u8   unLen;
    u8   i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
   
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))
 //   {   memcpy(p , ucComMF522Buf, 16);   }
    {
        for (i=0; i<16; i++)
        {    *(p +i) = ucComMF522Buf[i];   }
    }
    else
    {   status = MI_ERR;   }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：写数据到M1卡一块
//参数说明: addr[IN]：块地址
//          p [IN]：写入的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////                  
char PcdWrite(u8   addr,u8 *p )
{
    char   status;
    u8   unLen;
    u8   i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        //memcpy(ucComMF522Buf, p , 16);
        for (i=0; i<16; i++)
        {    
        	ucComMF522Buf[i] = *(p +i);   
        }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    
    return status;
}


/////////////////////////////////////////////////////////////////////
//功    能：命令卡片进入休眠状态
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdHalt(void)
{
    u8   status;
    u8   unLen;
    u8   ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//用MF522计算CRC16函数
/////////////////////////////////////////////////////////////////////
void CalulateCRC(u8 *pIn ,u8   len,u8 *pOut )
{
    u8   i,n;
    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
    {   WriteRawRC(FIFODataReg, *(pIn +i));   }
    WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do 
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOut [0] = ReadRawRC(CRCResultRegL);
    pOut [1] = ReadRawRC(CRCResultRegM);
}

/////////////////////////////////////////////////////////////////////
//功    能：通过RC522和ISO14443卡通讯
//参数说明：Command[IN]:RC522命令字
//          pIn [IN]:通过RC522发送到卡片的数据
//          InLenByte[IN]:发送数据的字节长度
//          pOut [OUT]:接收到的卡片返回数据
//          *pOutLenBit[OUT]:返回数据的位长度
/////////////////////////////////////////////////////////////////////
char PcdComMF522(u8   Command, 
                 u8 *pIn , 
                 u8   InLenByte,
                 u8 *pOut , 
                 u8 *pOutLenBit)
{
    char   status = MI_ERR;
    u8   irqEn   = 0x00;
    u8   waitFor = 0x00;
    u8   lastBits;
    u8   n;
    u16   i;
    switch (Command)
    {
        case PCD_AUTHENT:
													irqEn   = 0x12;
													waitFor = 0x10;
													break;
				case PCD_TRANSCEIVE:
													irqEn   = 0x77;
													waitFor = 0x30;
													break;
				default:	break;
    }
   
    WriteRawRC(ComIEnReg,irqEn|0x80);
    ClearBitMask(ComIrqReg,0x80);	//清所有中断位
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);	 	//清FIFO缓存
    
    for (i=0; i<InLenByte; i++)
    {   WriteRawRC(FIFODataReg, pIn [i]);    }
    WriteRawRC(CommandReg, Command);	  
//   	 n = ReadRawRC(CommandReg);
    
    if (Command == PCD_TRANSCEIVE)
    {    SetBitMask(BitFramingReg,0x80);  }	 //开始传送
    										 
      i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
// 	  i = 100000;
    do 
    {
        n = ReadRawRC(ComIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));
    ClearBitMask(BitFramingReg,0x80);

    if (i!=0)
    {    
        if(!(ReadRawRC(ErrorReg)&0x1B))
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {   status = MI_NOTAGERR;   }
            if (Command == PCD_TRANSCEIVE)
            {
               	n = ReadRawRC(FIFOLevelReg);
              	lastBits = ReadRawRC(ControlReg) & 0x07;
                if (lastBits)
                {   *pOutLenBit = (n-1)*8 + lastBits;   }
                else
                {   *pOutLenBit = n*8;   }
                if (n == 0)
                {   n = 1;    }
                if (n > MAXRLEN)
                {   n = MAXRLEN;   }
                for (i=0; i<n; i++)
                {   pOut [i] = ReadRawRC(FIFODataReg);    }
            }
        }
        else
        {   status = MI_ERR;   }
        
    }
   

    SetBitMask(ControlReg,0x80);           // stop timer now
    WriteRawRC(CommandReg,PCD_IDLE); 
    return status;
}

const u8 psw[]={0xff,0xff,0xff,0xff,0xff,0xff};
void RFID_Get_ID(u8 *pbuf)//卡号4字节
{
	uint8_t i;
	int8_t status;
	PcdReset();//复位RC522
	status=PcdRequest(PICC_REQALL,&RFID_DATA_BUF[0]);//寻天线区内未进入休眠状态的卡，返回卡片类型 2字节
	if(status!=MI_OK) return ;
	status=PcdAnticoll(&RFID_DATA_BUF[2]);//防冲撞，返回卡的序列号 4字节
	if(status!=MI_OK) return ;
	memcpy(pbuf,&RFID_DATA_BUF[2],4);//拷贝卡号
	status=PcdSelect(MLastSelectedSnr);//选卡
	if(status!=MI_OK) return ;
	status=PcdAuthState(PICC_AUTHENT1A,4,(u8*)psw,MLastSelectedSnr);//验证A密钥
	if(status!=MI_OK)
	{
		return ;
	}
	status=PcdRead(4,RFID_DATA_BUF);
	if(status!=MI_OK)
	{
		return ;
	}
}

u8 Compare(u8 *x,u8 *y,u8 len)
{
	 u8 m;
	for(m=0;m<len;m++)
		{
		 if((*(x+m))!=(*(y+m)))
			 {  
				return MI_ERR;
		 }
	}
	return MI_OK;
}

u8 Read_Block(uint8_t block,uint8_t *pbuf,uint8_t *psw)
{
	char status;
	PcdReset();//复位RC522
	status=PcdRequest(PICC_REQALL,&RFID_DATA_BUF[0]);//寻天线区内未进入休眠状态的卡，返回卡片类型 2字节
	if(status!=MI_OK)
	{
		return MI_ERR;
	}
	status=PcdAnticoll(&RFID_DATA_BUF[2]);//防冲撞，返回卡的序列号 4字节
	if(status!=MI_OK)
	{
		return MI_ERR;
	}
	memcpy(MLastSelectedSnr,&RFID_DATA_BUF[2],4);//从&RevBuffer[2]这个地址开始拷贝4个字节到指针起始位置MLastSelectedSnr
	status=PcdSelect(MLastSelectedSnr);//选卡
	if(status!=MI_OK)
	{
		return MI_ERR;
	}
	status=PcdAuthState(PICC_AUTHENT1A,block,psw,MLastSelectedSnr);//验证密匙
	if(status!=MI_OK)
	{
		return MI_ERR;
	}
	/*
	status=PcdWrite(addr,&cardData[0]);//把WriteData[]数组中的数据写到M1卡某一块中
	if(status!=MI_OK)
	{
		return MI_ERR;
	}
	*/
	status=PcdRead(block,pbuf);//从M1卡某一块读取到的数据存放在数组中
	if(status!=MI_OK)
	{
		return MI_ERR;
	}
	return MI_OK;
}

u8 Write_Block(uint8_t block,uint8_t *pbuf,uint8_t *psw)
{
	char status;
	
	PcdReset();//复位RC522
	status=PcdRequest(PICC_REQALL,&RFID_DATA_BUF[0]);//寻天线区内未进入休眠状态的卡，返回卡片类型 2字节
	if(status!=MI_OK)
	{
		return MI_ERR;
	}
    status=PcdAnticoll(&RFID_DATA_BUF[2]);//防冲撞，返回卡的序列号 4字节
    if(status!=MI_OK)
	{
		return MI_ERR;
	}
    memcpy(MLastSelectedSnr,&RFID_DATA_BUF[2],4);//从&RevBuffer[2]这个地址开始拷贝4个字节到指针起始位置MLastSelectedSnr
    status=PcdSelect(MLastSelectedSnr);//选卡
    if(status!=MI_OK)
	{
		return MI_ERR;
	}
	status=PcdAuthState(PICC_AUTHENT1A,block,psw,MLastSelectedSnr);//验证密匙
	if(status!=MI_OK)
	{
		return MI_ERR;
	}
	status=PcdWrite(block,pbuf);//把WriteData[]数组中的数据写到M1卡某一块中
	if(status!=MI_OK)
	{
		return MI_ERR;
	}
	status=PcdRead(block,XM);//从M1卡某一块读取到的数据存放在Read_Data[]数组中
	if(status!=MI_OK)
	{
		return MI_ERR;
	}
	while(Compare(pbuf,XM,8)!=MI_OK)
	{
		status=PcdAuthState(PICC_AUTHENT1A,block,psw,MLastSelectedSnr);//验证A密匙
		if(status!=MI_OK)
		{
			return MI_ERR;
		}
		status=PcdWrite(block,pbuf);//把WriteData[]数组中的数据写到M1卡某一块中
		if(status!=MI_OK)
		{
			return MI_ERR;
		}
		status=PcdRead(block,XM);//从M1卡某一块读取到的数据存放在Read_Data[]数组中
		if(status!=MI_OK)
		{
			return MI_ERR;
		}
	}
	if(PcdHalt()!=MI_OK)
	{
		return MI_ERR;
  }
	return MI_OK;    
}
