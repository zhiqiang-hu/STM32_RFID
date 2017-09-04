/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : usb_endp.c
* Author             : MCD Application Team
* Version            : V3.0.1
* Date               : 04/27/2009
* Description        : Endpoint routines
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/

#include "hw_config.h"
#include "usb_lib.h"
#include "usb_istr.h"
#include "usart.h"
#include "led.h"
#include "usbio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : EP1_IN_Callback.
* Description    : EP1 IN Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_OUT_Callback(void)
{
  /* Read received data */  
  #if 1
  usb_endp1.rx_cnt = USB_SIL_Read(EP1_OUT, usb_endp1.rx_buff);
  SetEPRxStatus(ENDP1, EP_RX_VALID);
  #endif
 
}
/*******************************************************************************
* Function Name  : EP1_OUT_Callback.
* Description    : EP1 OUT Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP2_OUT_Callback(void)
{
  /* Read received data */  
  #if 1
  usb_endp2.rx_cnt = USB_SIL_Read(EP2_OUT, usb_endp2.rx_buff);
//  SetEPRxStatus(ENDP2, EP_RX_VALID);
 	#endif
}
/*******************************************************************************
* Function Name  : EP1_IN_Callback.
* Description    : EP1 IN Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback(void)
{
	usb_endp1.tx_ok = 1;
//  printf("EP1_IN\n");
}

/*******************************************************************************
* Function Name  : EP2_IN_Callback.
* Description    : EP2 IN Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP2_IN_Callback(void)
{
	usb_endp2.tx_ok = 1;
//	printf("EP2 IN:\n");
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/

