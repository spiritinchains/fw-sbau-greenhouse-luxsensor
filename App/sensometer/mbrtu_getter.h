/*
 * mbrtu_getter.h
 *
 *  Created on: Feb 25, 2024
 *      Author: Sameen
 */

#ifndef SENSOMETER_MBRTU_GETTER_H_
#define SENSOMETER_MBRTU_GETTER_H_


#include "config.h"

#if USE_MBRTU && USE_MBRTU_GETTER && USE_MBRTU_GATEWAY

#include <modbus/client.h>
#include <sensometer/mbrtu_gateway.h>


typedef struct _mbrtu_getter mbrtu_getter_t;
typedef struct _mbrtu_getter_node mbrtu_getter_node_t;


struct _mbrtu_getter_node
{
	const uint8_t id;
	const mb_req_t req;

	void (*callback) (mbrtu_getter_node_t *, void *);
	void *receiver;

	uint8_t timeout_count;

	uint32_t interval_ms;
	uint32_t last_send_time_ms;

	uint8_t rsp[256];
	uint16_t rsp_len;
	bool rsp_received;
};


struct _mbrtu_getter
{
	mbrtu_gateway_t *gwy;

	mbrtu_getter_node_t *nodes;
	uint8_t node_current_i;
	uint8_t num_nodes;
	uint8_t state;

	void (*update_cb) (void);
};


void
mbrtu_getter_process (mbrtu_getter_t *getter);

#endif /* USE_MBRTU && USE_MBRTU_GETTER && USE_MBRTU_GATEWAY */

#endif /* SENSOMETER_MBRTU_GETTER_H_ */
