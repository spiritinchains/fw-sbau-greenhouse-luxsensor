/*
 * rs485.h
 *
 *  Created on: Oct 9, 2023
 *      Author: Sameen
 */

#ifndef RS485_H_
#define RS485_H_

#include "config.h"

#if USE_RS485
#include <stdint.h>
#include <usart.h>


struct _rs485
{
	UART_HandleTypeDef 	*uart;
	GPIO_TypeDef		*dir_port;
	uint16_t			dir_pin;

	void *observer;
	void (*recv_callback)(void *obs, uint8_t *data, uint16_t size);
};

typedef struct _rs485 rs485_t;

extern rs485_t rs485_ch[];


void rs485_isr (rs485_t *ch);
void rs485_send (rs485_t *ch, uint8_t *data, uint16_t size);


#endif /* USE_RS485 */

#endif /* RS485_H_ */
