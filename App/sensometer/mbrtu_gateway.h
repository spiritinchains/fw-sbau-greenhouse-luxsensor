/*
 * mbrtu_gateway.h
 *
 *  Created on: Feb 19, 2024
 *      Author: Sameen
 */

#ifndef SENSOMETER_MBRTU_GATEWAY_H_
#define SENSOMETER_MBRTU_GATEWAY_H_

#include "config.h"

#if USE_MBRTU && USE_MBRTU_GATEWAY
#include "mbrtu.h"

#include <stdint.h>

#include <common/queue.h>


typedef struct _mbrtu_gateway mbrtu_gateway_t;
typedef struct _mbrtu_gateway_query mbrtu_gateway_query_t;


typedef enum
{
	GWY_STATE_SENDREQ,
	GWY_STATE_RECVRSP
} mbrtu_gateway_state_t;



struct _mbrtu_gateway_query
{
	uint8_t req[MBRTU_GATEWAY_REQ_SIZE];
	uint16_t req_len;

	void *rsp_receiver;
	void (*rsp_callback)(void *receiver, uint8_t *rsp, uint16_t rsp_len);
	uint32_t timeout;
};


typedef QUEUE_T(mbrtu_gateway_query_t, MBRTU_GATEWAY_QUEUE_MAXSIZE) mbrtu_gateway_queue_t;


struct _mbrtu_gateway
{
	mbrtu_gateway_state_t state;

	mbrtu_gateway_queue_t queue;
	mbrtu_gateway_queue_t advance_queue;

	mbrtu_gateway_queue_t *cur_queue;
	mbrtu_gateway_query_t *cur_query;

	mbrtu_t *mbrtu;

	uint32_t time_last_req_sent;
	uint32_t time_last_rsp_received;
};



void
mbrtu_gateway_init (mbrtu_gateway_t *gwy, mbrtu_t *ch);

void
mbrtu_gateway_process (mbrtu_gateway_t *gwy);

int
mbrtu_gateway_send_queue (mbrtu_gateway_t *gwy, mbrtu_gateway_query_t *query);

int
mbrtu_gateway_send_now (mbrtu_gateway_t *gwy, mbrtu_gateway_query_t *query);

#endif /* USE_MBRTU && USE_MBRTU_GATEWAY */

#endif /* SENSOMETER_MBRTU_GATEWAY_H_ */
