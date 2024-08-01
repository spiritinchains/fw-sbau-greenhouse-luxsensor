/*
 * mbrtu.c
 *
 *  Created on: Oct 9, 2023
 *      Author: Sameen
 */



//#define LOG_LEVEL_LOCAL LOG_TRACE
#define LOG_SUBSYSTEM "mbrtu"

#include "mbrtu.h"

#if USE_MBRTU
#include <common/log.h>
#include <errno.h>



mbrtu_t mbrtu_ch[MBRTU_NUM_IFACES];


void mbrtu_on_recv (void*, uint8_t*, uint16_t);
void mbrtu_on_t1_5_done (mbrtu_t*);
void mbrtu_on_t3_5_done (mbrtu_t*);

void mbrtu_tim_start (mbrtu_t*);
void mbrtu_tim_stop (mbrtu_t*);


void
mbrtu_init (mbrtu_t *mbrtu, rs485_t *ch, TIM_HandleTypeDef *tim)
{
	ch->observer = mbrtu;
	ch->recv_callback = mbrtu_on_recv;

	mbrtu->channel = ch;
	mbrtu->tim = tim;

	mbrtu->recv_i = 0;
	mbrtu->recv_cplt = false;

	mbrtu->send_clear = false;

	UART_InitTypeDef *uart_params = &(mbrtu->channel->uart->Init);

	uint32_t baud = uart_params->BaudRate;

	// TODO: handle other baud rates
#ifdef MBRTU_TIMING_STRICT
	if (baud == 9600)
	{
		mbrtu->t1_5 = 1407;
		mbrtu->t3_5 = 3282;
	}
	else if (baud == 115200)
	{
		mbrtu->t1_5 = 750;
		mbrtu->t3_5 = 1750;
	}
#else
	(void) baud;

	mbrtu->t1_5 = 2000;
	mbrtu->t3_5 = 3000;
#endif

	mbrtu->state = MBRTU_INIT;

	__HAL_UART_CLEAR_FLAG(ch->uart, (USART_SR_ORE | USART_SR_PE | USART_SR_FE));
	__HAL_UART_ENABLE_IT(ch->uart, UART_IT_RXNE);
	__HAL_TIM_ENABLE_IT(tim, TIM_IT_CC1 | TIM_IT_CC2);
	mbrtu_tim_start(mbrtu);
}



ssize_t
mbrtu_read (mbrtu_t *mbrtu, void *buf, size_t cnt)
{
	size_t n = cnt;

	if (n > mbrtu->recv_i)
	{
		n = mbrtu->recv_i;
	}

	if (mbrtu->recv_cplt)
	{
		for (int i = 0; i < n; i++)
		{
			((uint8_t*)buf)[i] = mbrtu->recv_buf[i];
		}
		mbrtu->recv_i = 0;
		mbrtu->recv_cplt = false;
		return n;
	}
	else
	{
		// TODO: set errno appropriately
		return -1;
	}
}



ssize_t
mbrtu_write (mbrtu_t *mbrtu, void *buf, size_t cnt)
{
	size_t n = cnt;

	if (n > 256)
	{
		n = 256;
	}

	if (mbrtu->send_clear)
	{
		rs485_send(mbrtu->channel, buf, n);
		mbrtu_tim_start(mbrtu);
		return n;
	}
	else
	{
		// TODO: set errno appropriately
		return -1;
	}
}



void
mbrtu_on_recv (void *obs, uint8_t *data, uint16_t len)
{
	mbrtu_t *mbrtu = (mbrtu_t*)obs;

	if (mbrtu->recv_cplt)
	{
		return;
	}

	if (mbrtu->state == MBRTU_CTRL_WAIT)
	{
		mbrtu->state = MBRTU_NOK;
		return;
	}
	else if (mbrtu->state != MBRTU_IDLE && mbrtu->state != MBRTU_RECV)
	{
		return;
	}

	if (mbrtu->recv_i + len <= 256)
	{
		for (int i = 0; i < len; i++)
		{
			mbrtu->recv_buf[mbrtu->recv_i + i] = data[i];
		}
		mbrtu->recv_i += len;
	}

	mbrtu->state = MBRTU_RECV;

	// Disable sending and receiving data
	mbrtu->recv_cplt = false;
	mbrtu->send_clear = false;

	mbrtu_tim_start(mbrtu);
}



void
mbrtu_on_t1_5_done (mbrtu_t *mbrtu)
{
	if (mbrtu->state == MBRTU_RECV)
	{
		mbrtu->state = MBRTU_CTRL_WAIT;
	}
}



void
mbrtu_on_t3_5_done (mbrtu_t *mbrtu)
{
	if (mbrtu->state == MBRTU_INIT || mbrtu->state == MBRTU_EMIT || mbrtu->state == MBRTU_NOK)
	{
		mbrtu->state = MBRTU_IDLE;
		mbrtu->send_clear = true;
	}
	else if (mbrtu->state == MBRTU_CTRL_WAIT)
	{
		mbrtu->state = MBRTU_IDLE;
		mbrtu->send_clear = true;
		mbrtu->recv_cplt = true;
	}
}



void
mbrtu_tim_start (mbrtu_t *mbrtu)
{
	TIM_TypeDef *timer = mbrtu->tim->Instance;

	timer->CR1 &= ~TIM_CR1_CEN;		// disable timer counter
	timer->EGR |= TIM_EGR_UG;		// re-initialize counter and update registers
	timer->CNT = 0;					// reset counter value

	// Channel 1 = 1.5chr
	timer->CCR1 = mbrtu->t1_5;
	// Channel 2 = 3.5chr
	timer->CCR2 = mbrtu->t3_5;

	// Channel 3 and 4 are unused; set to out-of-range value
	timer->CCR3 = 65535;
	timer->CCR4 = 65535;

	timer->CR1 |= TIM_CR1_CEN;		// reenable timer counter
}



void
mbrtu_tim_stop (mbrtu_t *mbrtu)
{
	TIM_TypeDef *timer = mbrtu->tim->Instance;

	timer->CR1 &= ~TIM_CR1_CEN;		// disable timer counter
	timer->EGR |= TIM_EGR_UG;		// re-initialize counter and update registers
	timer->CNT = 0;					// reset counter value
}



void
mbrtu_tim_isr (mbrtu_t *mbrtu)
{
	TIM_TypeDef *tim = mbrtu->tim->Instance;

	// Channel 1 OC interrupt
	if (__HAL_TIM_GET_FLAG(mbrtu->tim, TIM_FLAG_CC1) != 0)
	{
		if (__HAL_TIM_GET_IT_SOURCE(mbrtu->tim, TIM_FLAG_CC1) != 0)
		{
			if ((tim->CCMR1 & TIM_CCMR1_CC1S) == 0)
			{
				mbrtu_on_t1_5_done(mbrtu);
			}
		}
	}

	// Channel 2 OC interrupt
	if (__HAL_TIM_GET_FLAG(mbrtu->tim, TIM_FLAG_CC2) != 0)
	{
		if (__HAL_TIM_GET_IT_SOURCE(mbrtu->tim, TIM_FLAG_CC2) != 0)
		{
			if ((tim->CCMR1 & TIM_CCMR1_CC2S) == 0)
			{
				mbrtu_on_t3_5_done(mbrtu);
				mbrtu_tim_stop(mbrtu);
			}
		}
	}

	/*
	 * NOTE: Cleanup of registers is left to HAL IRQ Handler. If HAL is ever
	 * replaced/changed, we need to ensure the registers are still being
	 * cleared.
	 */
}


#endif /* USE_MBRTU */

