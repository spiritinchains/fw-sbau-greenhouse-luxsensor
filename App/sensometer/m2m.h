/*
 * m2m_legacy.h
 *
 *  Created on: Feb 5, 2024
 *      Author: Sameen
 */

#ifndef SENSOMETER_M2M_H_
#define SENSOMETER_M2M_H_

#include "config.h"

#if USE_M2M

#include <usart.h>


void
m2m_init (UART_HandleTypeDef *uart);

void
m2m_isr (void);

void
m2m_process (void);

#if M2M_USE_LEGACY

void
m2m_send_payload(uint8_t *payload, size_t payload_size);

#endif /* M2M_USE_LEGACY */

#endif /* USE_M2M */

#endif /* SENSOMETER_M2M_H_ */
