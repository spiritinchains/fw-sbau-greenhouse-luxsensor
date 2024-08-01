/*
 * mbrtu_slave.h
 *
 *  Created on: Oct 22, 2023
 *      Author: Sameen
 */

#ifndef SENSOMETER_MBRTU_SLAVE_H_
#define SENSOMETER_MBRTU_SLAVE_H_


#include "config.h"

#if USE_MBRTU && USE_MBRTU_SLAVE
#include "mbrtu.h"


struct _mbrtu_slave
{
	mbrtu_t *base;
	uint8_t *id;
	uint8_t req[256];
};

typedef struct _mbrtu_slave mbrtu_slave_t;


extern uint8_t slave_id;


void mbrtu_slave_init (mbrtu_slave_t *mbs, mbrtu_t *mbrtu);
void mbrtu_slave_process (mbrtu_slave_t *mbs);


#endif /* USE_MBRTU && USE_MBRTU_SLAVE */

#endif /* SENSOMETER_MBRTU_SLAVE_H_ */
