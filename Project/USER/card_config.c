#include "card_config.h"

uint8_t buff[512];
const int8_t VERSION[8] =
{
	version_master,version_second, \
	version_build,version_mcu_type, \
	version_year,version_month, \
	version_day,version_hour
};


const uint8_t DEVICE_SERIAL[] =
{
		0x00,0xC1,0xB1,0x50,0xA8,0x0B,0x5E,0x9A,
		0xDE,0xE5,0x78,0x5D,0xFE,0x7B,0x56,0x66
};









