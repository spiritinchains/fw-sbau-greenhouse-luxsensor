/*
 * rs485.c
 *
 *  Created on: Oct 9, 2023
 *      Author: Sameen
 */

#define LOG_SUBSYSTEM "rs485"

#include "rs485.h"

#if USE_RS485
#include <common/log.h>


rs485_t rs485_ch[RS485_NUM_IFACES];


void
rs485_send (rs485_t *ch, uint8_t *data, uint16_t size)
{
	if (ch->dir_port)
		HAL_GPIO_WritePin(ch->dir_port, ch->dir_pin, GPIO_PIN_SET);

	HAL_UART_Transmit(ch->uart, data, size, 1000);

	if (ch->dir_port)
		HAL_GPIO_WritePin(ch->dir_port, ch->dir_pin, GPIO_PIN_RESET);
}



void
rs485_isr (rs485_t *ch)
{
	USART_TypeDef *uart = ch->uart->Instance;
	uint8_t stat = uart->SR;
	uint8_t data = uart->DR;

	if (stat & (USART_SR_ORE | USART_SR_PE | USART_SR_FE))
	{
		__HAL_UART_ENABLE_IT(ch->uart, UART_IT_RXNE);
	}

	if (ch->observer && ch->recv_callback)
	{
		ch->recv_callback(ch->observer, &data, 1);
	}
}


#endif /* USE_RS485 */
