#include "usbio.h"
#include "delay.h"
#include "usart.h"
#include "string.h"
#include "timer.h"

uint16_t endp_tx_rx_sta = 0;
USB_ENDP_STRUCT usb_endp1;
USB_ENDP_STRUCT usb_endp2;

uint8_t WritePort1(uint32_t len,uint8_t *sendbuff)
{
	uint32_t timeout;
	uint32_t sendcnt = 0,tmp;	
	while(1)
	{
		tmp = len - sendcnt;
		if(tmp >= 16)
		{
			tmp = USB_SIL_Write(ENDP1,sendbuff+sendcnt,16);		
		}
		else
		{
			tmp = USB_SIL_Write(ENDP1,sendbuff+sendcnt,tmp);
		}
		SetEPTxValid(ENDP1);
		while(!usb_endp1.tx_ok)
		{
			timeout++;
			if(timeout>60000)
			{
				return 1;
			}
		}
		usb_endp1.tx_ok = 0;
		sendcnt = sendcnt + tmp;
		if (sendcnt >= len)									
		{
			break;
		}	
	}
	return 0;
}

uint8_t WritePort2(uint32_t len,uint8_t *sendbuff)
{
	uint32_t timeout=0;
	uint32_t sendcnt = 0,tmp;	
	while(1)
	{
		tmp = len - sendcnt;
		if(tmp >= 64)
		{
			tmp = USB_SIL_Write(ENDP2,sendbuff+sendcnt,64);		
		}
		else
		{
			tmp = USB_SIL_Write(ENDP2,sendbuff+sendcnt,tmp);
		}
		SetEPTxValid(ENDP2);
		while(!usb_endp2.tx_ok)
		{
			timeout++;
			if(timeout>60000)
			{
				return 1;
			}
		}
		timeout = 0;
		usb_endp2.tx_ok = 0;
		sendcnt = sendcnt + tmp;
		if (sendcnt >= len)									
		{
			break;
		}	
	}
	return 0;
}



uint8_t ReadPort1(uint32_t len,uint8_t *recbuff)
{
	if(usb_endp1.rx_cnt)
	{
		memcpy(recbuff,usb_endp1.rx_buff,len);
		usb_endp1.rx_cnt = 0;
//		SetEPRxStatus(ENDP1, EP_RX_VALID);
		return 0;
	}
	return 1;
}

uint8_t ReadPort2(uint32_t len,uint8_t *pbuff)
{
	uint8_t i;
	uint32_t num=len;
	Reset_System_Time();//防止上位机出故障后程序卡死在这里。
	while(num>0)
	{
		if(Get_System_Time()>15)//100ms*15 1.5秒超时时间
		{
			return 1;
		}
		if(usb_endp2.rx_cnt)
		{
			
			memcpy(pbuff,usb_endp2.rx_buff,usb_endp2.rx_cnt);
			pbuff+=usb_endp2.rx_cnt;
			num-= usb_endp2.rx_cnt;
//			for(i=0;i<usb_endp2.rx_cnt;i++)
//			{
//				printf("%02X ",pbuff[i]);
//			}
			usb_endp2.rx_cnt = 0;
//			printf("num = %d\n",num);
			SetEPRxStatus(ENDP2, EP_RX_VALID);
			Reset_System_Time();
		}
	}
	return 0;
}




