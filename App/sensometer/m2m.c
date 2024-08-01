/*
 * m2m.c
 *
 *  Created on: Feb 5, 2024
 *      Author: Sameen
 */

#define LOG_SUBSYSTEM "m2m"

#include "m2m.h"

#if USE_M2M

#include <stdbool.h>
#include <string.h>

#include <usart.h>

#include <common/log.h>
#include <modbus/server.h>
#include <modbus/util.h>
#include <sensometer/webcomm.h>
#include <sensometer/mbrtu_slave.h>


static UART_HandleTypeDef* m2m_uart;

static volatile uint8_t m2m_buf[M2M_BUF_SIZE];
static volatile size_t m2m_buf_i = 0;
static volatile uint32_t m2m_last_update = 0;


#if M2M_USE_LEGACY


bool m2m_is_packet_valid (uint8_t*, size_t);
void m2m_process_payload (uint8_t*, size_t);

void m2m_handle_general_query (uint8_t*, size_t);
void m2m_handle_general_response (uint8_t*, size_t);
void m2m_handle_general_data (uint8_t*, size_t);
void m2m_handle_command (uint8_t*, size_t);

void m2m_send_payload(uint8_t*, size_t);


void
m2m_init (UART_HandleTypeDef *uart)
{
	m2m_uart = uart;

	__HAL_UART_CLEAR_FLAG(uart, (USART_SR_ORE | USART_SR_PE | USART_SR_FE));
	__HAL_UART_ENABLE_IT(uart, UART_IT_RXNE);
}



void
m2m_process (void)
{
	if (m2m_buf_i == 0)
	{
		return;
	}

	// check for end of transfer
	if (HAL_GetTick() - m2m_last_update < 5)
	{
		return;
	}

	// check if data in buffer is of a valid packet
	if (!m2m_is_packet_valid((uint8_t*)m2m_buf, m2m_buf_i))
	{
		LOG(LOG_INFO, "Invalid Packet Received");
		m2m_buf_i = 0;
		return;
	}

	LOG(LOG_DEBUG, "ESP Packet Received");
	LOG_PRINTBUF(LOG_DEBUG, (uint8_t*)m2m_buf, m2m_buf_i);

	// get payload size
	uint16_t psize = 0;
	psize |= m2m_buf[1];
	psize <<= 8;
	psize |= m2m_buf[2];

	m2m_process_payload((uint8_t*)&m2m_buf[3], psize);

	// clear buffer
	m2m_buf_i = 0;
}



bool
m2m_is_packet_valid (uint8_t* pkt, size_t pkt_size)
{
	uint16_t crc = crc16_compute(pkt, pkt_size);

	if (crc != 0)
	{
		// crc error
		return false;
	}

	if (pkt[0] != 0x80)
	{
		// invalid start byte
		return false;
	}

	return true;
}



void
m2m_process_payload(uint8_t* payload, size_t payload_size)
{
	switch (payload[0])
	{
	case 0x01:
		/* General Query */
		LOG(LOG_DEBUG, "Payload type 01: General Query");
		m2m_handle_general_query(&payload[1], payload_size - 1);
		break;

	case 0x02:
		/* General Response */
		LOG(LOG_DEBUG, "Payload type 02: General Response");
		m2m_handle_general_response(&payload[1], payload_size - 1);
		break;

	case 0x03:
		/* General Data */
		LOG(LOG_DEBUG, "Payload type 03: General Data");
		m2m_handle_general_data(&payload[1], payload_size - 1);
		break;

	case 0x04:
		/* Command */
		LOG(LOG_DEBUG, "Payload type 04: Command");
		m2m_handle_command(&payload[1], payload_size - 1);
		break;
	default:
		LOG(LOG_DEBUG, "Unsupprted m2m payload type: %02X", payload[0]);
		break;
	}
}



void
m2m_handle_general_query(uint8_t *query, size_t size)
{
	// TODO: unimplemented
}



void
m2m_handle_general_response(uint8_t *resp, size_t size)
{
	// TODO: unimplemented
}



void
m2m_handle_general_data(uint8_t *data, size_t size)
{
	webcomm_pkt_t pkt;
	webcomm_parse_json((char*)data, &pkt);

	if (strncmp(pkt.serial, DEVICE_SERIAL_ID, 15) == 0)
	{
		webcomm_process_packet(&pkt);
	}
}



void
m2m_handle_command(uint8_t *command, size_t size)
{
	//
}



void
m2m_send_payload(uint8_t *payload, size_t payload_size)
{
	if (payload_size > (M2M_BUF_SIZE - 5))
	{
		// payload too large
		return;
	}

	uint8_t packet[M2M_BUF_SIZE];

	packet[0] = 0x80;
	packet[1] = (payload_size >> 8) & 0xFF;
	packet[2] = payload_size & 0xFF;

	memcpy(&packet[3], payload, payload_size);

	uint16_t crc = crc16_compute(packet, payload_size + 3);

	packet[payload_size + 3] = crc & 0xFF;
	packet[payload_size + 4] = (crc >> 8) & 0xFF;

	HAL_UART_Transmit(m2m_uart, packet, payload_size + 5, HAL_MAX_DELAY);

	return;
}


#endif /* M2M_USE_LEGACY */



void
m2m_isr (void)
{
	uint8_t data = m2m_uart->Instance->DR;
	uint8_t stat = m2m_uart->Instance->SR;

	if (stat & (USART_SR_ORE | USART_SR_PE | USART_SR_FE))
	{
		__HAL_UART_ENABLE_IT(m2m_uart, UART_IT_RXNE);
	}

	m2m_buf[m2m_buf_i] = data;
	m2m_buf_i++;
	m2m_last_update = HAL_GetTick();
}


#endif /* USE_M2M */
