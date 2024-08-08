/*
 * app_main.c
 *
 *  Created on: Aug 1, 2024
 *      Author: sameen
 */

#include <main.h>
#include <stdbool.h>
#include <i2c.h>
#include <iwdg.h>
#include <max44009.h>

#include <common/log.h>
#include <sensometer/rs485.h>
#include <sensometer/mbrtu.h>
#include <sensometer/mbrtu_slave.h>


bool get_lux_flag = false;

uint8_t LUX_READ1[] = { 0x03 };
uint8_t LUX_READ2[] = { 0x04 };

uint16_t luxval_high = 0;
uint16_t luxval_low = 0;

mbrtu_slave_t slave_channel;


void
init (void)
{
	LOG(LOG_DEBUG, "lux sensor start\n");

	rs485_ch[0].dir_port = RS485_DE_GPIO_Port;
	rs485_ch[0].dir_pin = RS485_DE_Pin;
	rs485_ch[0].uart = &huart1;

	MAX44009_Init(&hi2c1);

	mbrtu_init(&mbrtu_ch[0], &rs485_ch[0], &htim3);
	mbrtu_slave_init(&slave_channel, &mbrtu_ch[0]);
}



void
loop (void)
{
	mbrtu_slave_process(&slave_channel);

	if (get_lux_flag == true)
	{
		float temp;
		MAX44009_ReadLightHighResolution(&temp);
		int32_t luxval = (int32_t) temp;
		LOG(LOG_DEBUG, "Lux: %d", luxval);
		luxval_high = (luxval & 0xFFFF0000) >> 16;
		luxval_low = luxval & 0xFFFF;
		get_lux_flag = false;

	}

	HAL_IWDG_Refresh(&hiwdg);
}



void
tick_handler (void)
{
	if (HAL_GetTick() % 500 == 0)
	{
		get_lux_flag = true;
	}
}
