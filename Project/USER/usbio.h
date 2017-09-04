#ifndef __USB_IO_H
#define __USB_IO_H

#include "hw_config.h"
#include "usb_lib.h"
#include "usb_pwr.h"

typedef struct ENDP_STRUCT
{
	uint8_t rx_cnt;
	uint8_t tx_ok;
	uint8_t len;
	uint8_t rx_buff[64];
}USB_ENDP_STRUCT,*pUSB_ENDP_STRUCT;



extern USB_ENDP_STRUCT usb_endp1;
extern USB_ENDP_STRUCT usb_endp2;

uint8_t WritePort1(uint32_t len,uint8_t *sendbuff);
uint8_t WritePort2(uint32_t len,uint8_t *sendbuff);
uint8_t ReadPort1(uint32_t len,uint8_t *recbuff);
uint8_t ReadPort2(uint32_t len,uint8_t *recbuff);



//≤‚ ‘”√
void USB_SendData(uint8_t* pBufferPointer, uint32_t wBufferSize);








#endif
