/*
 * mbrtu.h
 *
 *  Created on: Oct 9, 2023
 *      Author: Sameen
 */

#ifndef MBRTU_H_
#define MBRTU_H_


#include "config.h"

#if USE_MBRTU
#include "rs485.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

#include <main.h>
#include <tim.h>


typedef enum
{
	MBRTU_INIT,
	MBRTU_IDLE,
	MBRTU_EMIT,
	MBRTU_CTRL_WAIT,
	MBRTU_NOK,
	MBRTU_RECV
} mbrtu_state_t;



struct _mbrtu
{
	rs485_t *channel;
	TIM_HandleTypeDef *tim;

	// timer intervals
	uint32_t t1_5;
	uint32_t t3_5;

	mbrtu_state_t state;

	uint8_t recv_buf[MBRTU_BUF_SIZE];
	uint8_t recv_i;
	bool recv_cplt;
	bool send_clear;

};

typedef struct _mbrtu mbrtu_t;

extern mbrtu_t mbrtu_ch[];


void mbrtu_init (mbrtu_t *mbrtu, rs485_t *ch, TIM_HandleTypeDef *tim);

ssize_t mbrtu_read (mbrtu_t *mbrtu, void *buf, size_t cnt);
ssize_t mbrtu_write (mbrtu_t *mbrtu, void *buf, size_t cnt);

void mbrtu_tim_isr (mbrtu_t *mbrtu);


#endif /* USE_MBRTU */

#endif /* MBRTU_H_ */
