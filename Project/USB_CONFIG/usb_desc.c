/**
  ******************************************************************************
  * @file    usb_desc.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Descriptors for Custom HID Demo
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* device descriptor: test device type */






/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/






/* USB Standard Device Descriptor */
const uint8_t CustomHID_DeviceDescriptor[CUSTOMHID_SIZ_DEVICE_DESC] =
  {
    0x12,                       /*bLength */
    USB_DEVICE_DESCRIPTOR_TYPE, /*bDescriptorType*/
    0x00,                       /*bcdUSB */
    0x01,
    USB_CLASS_CODE_TEST_CLASS_DEVICE,                       /*bDeviceClass*/
    0x00,                       /*bDeviceSubClass*/
    0x00,                       /*bDeviceProtocol*/
    0x40,                       /*bMaxPacketSize40*/
    0x71,                       /*idVendor (0x0483)*/
    0x04,
    0x78,                       /*idProduct = 0x5750*/
    0x23,
    0x00,                       /*bcdDevice rel. 1.00*/
    0x01,
    0,                          /*Index of string descriptor describing
                                              manufacturer */
    0,                          /*Index of string descriptor describing
                                             product*/
    0,                          /*Index of string descriptor describing the
                                             device serial number */
    0x01                        /*bNumConfigurations*/
  }
  ; /* CustomHID_DeviceDescriptor */


/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const uint8_t CustomHID_ConfigDescriptor[CUSTOMHID_SIZ_CONFIG_DESC] =
  {
    0x09, /* bLength: Configuration Descriptor size */
    USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
    CUSTOMHID_SIZ_CONFIG_DESC,
    /* wTotalLength: Bytes returned */
    0x00,
    0x01,         /* bNumInterfaces: 1 interface */
    0x01,         /* bConfigurationValue: Configuration value */
    0x00,         /* iConfiguration: Index of string descriptor describing
                                 the configuration*/
    0x60,         /* bmAttributes: Self powered */
    0x32,         /* MaxPower 100 mA: this current is used for detecting Vbus */

    /************** Descriptor of Custom HID interface ****************/
    /* 09 */
    0x09,         /* bLength: Interface Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE,/* bDescriptorType: Interface descriptor type */
    0x00,         /* bInterfaceNumber: Number of Interface */
    0x00,         /* bAlternateSetting: Alternate setting */
    NUM_ENDPOINTS,         /* bNumEndpoints */
    USB_CLASS_CODE_TEST_CLASS_DEVICE,         /* bInterfaceClass: HID */
    USB_SUBCLASS_CODE_TEST_CLASS_D12,         /* bInterfaceSubClass : 1=BOOT, 0=no boot */
    USB_PROTOCOL_CODE_TEST_CLASS_D12,         /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
    0,            /* iInterface: Index of string descriptor */

    /******************** Descriptor of Custom HID endpoints ******************/
    /* 18 */
    0x07,          /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: */

    0x81,          /* bEndpointAddress: Endpoint Address (IN) */
    USB_ENDPOINT_TYPE_INTERRUPT,          /* bmAttributes: Interrupt endpoint */
    EP1_PACKET_SIZE,          /* wMaxPacketSize: 2 Bytes max */
    0x00,
    0x01,          /* bInterval: Polling Interval (1 ms) */
    /* 26 */
    	
    0x07,	/* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType: */
			/*	Endpoint descriptor type */
    0x01,	/* bEndpointAddress: */
			/*	Endpoint Address (OUT) */
    USB_ENDPOINT_TYPE_INTERRUPT,	/* bmAttributes: Interrupt endpoint */
    EP1_PACKET_SIZE,	/* wMaxPacketSize: 2 Bytes max  */
    0x00,
    0x01,	/* bInterval: Polling Interval (1 ms) */

	0x07,	/* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType: */
			/*	Endpoint descriptor type */
    0x82,	/* bEndpointAddress: */
			/*	Endpoint Address (IN) */
    USB_ENDPOINT_TYPE_BULK,	/* bmAttributes: Interrupt endpoint */
    EP2_PACKET_SIZE,	/* wMaxPacketSize: 64 Bytes max  */
    0x00,
    10,	/*批量传输时该值无效 */

	0x07,	/* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType: */
			/*	Endpoint descriptor type */
    0x02,	/* bEndpointAddress: */
			/*	Endpoint Address (OUT) */
    USB_ENDPOINT_TYPE_BULK,	/* bmAttributes: Interrupt endpoint */
    EP2_PACKET_SIZE,	/* wMaxPacketSize: 64 Bytes max  */
    0x00,
    10,	/*批量传输时该值无效 */
    /* 46 */
  }
  ; /* CustomHID_ConfigDescriptor */


/* USB String Descriptors (optional) */
const uint8_t CustomHID_StringLangID[CUSTOMHID_SIZ_STRING_LANGID] =
  {
    CUSTOMHID_SIZ_STRING_LANGID,
    USB_STRING_DESCRIPTOR_TYPE,
    0x09,
    0x04
  }
  ; /* LangID = 0x0409: U.S. English */

const uint8_t CustomHID_StringVendor[CUSTOMHID_SIZ_STRING_VENDOR] =
  {
    CUSTOMHID_SIZ_STRING_VENDOR, /* Size of Vendor string */
    USB_STRING_DESCRIPTOR_TYPE,  /* bDescriptorType*/
    /* Manufacturer: "STMicroelectronics" */
    'S', 0, 'T', 0, 'M', 0, 'i', 0, 'c', 0, 'r', 0, 'o', 0, 'e', 0,
    'l', 0, 'e', 0, 'c', 0, 't', 0, 'r', 0, 'o', 0, 'n', 0, 'i', 0,
    'c', 0, 's', 0
  };

const uint8_t CustomHID_StringProduct[CUSTOMHID_SIZ_STRING_PRODUCT] =
  {
    CUSTOMHID_SIZ_STRING_PRODUCT,          /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    'S', 0, 'T', 0, 'M', 0, '3', 0, '2', 0, ' ', 0, 'C', 0,
    'u', 0, 's', 0, 't', 0, 'm', 0, ' ', 0, 'H', 0, 'I', 0,
    'D', 0
  };
uint8_t CustomHID_StringSerial[CUSTOMHID_SIZ_STRING_SERIAL] =
  {
    CUSTOMHID_SIZ_STRING_SERIAL,           /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    'S', 0, 'T', 0, 'M', 0,'3', 0,'2', 0
  };

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

