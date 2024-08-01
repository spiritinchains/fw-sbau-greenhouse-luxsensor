/*
 * mbrtu_dispatcher.c
 *
 *  Created on: Feb 25, 2024
 *      Author: Sameen
 */

#define LOG_SUBSYSTEM "mbrtu_getter"

#include "mbrtu_getter.h"

#if USE_MBRTU && USE_MBRTU_GETTER && USE_MBRTU_GATEWAY

#include <common/log.h>


enum {
	GETTER_SEND_REQ,
	GETTER_WAIT_FOR_RSP
};


static int send_request (mbrtu_getter_t*);
static int wait_for_response (mbrtu_getter_t*);

static void response_handler (void*, uint8_t*, uint16_t);


void
mbrtu_getter_process (mbrtu_getter_t *getter)
{
	switch (getter->state)
	{
	case GETTER_SEND_REQ:
		getter->state = send_request(getter);
		break;

	case GETTER_WAIT_FOR_RSP:
		getter->state = wait_for_response(getter);
		break;

	default:
		getter->state = GETTER_SEND_REQ;
		break;
	}
}



static mbrtu_getter_node_t*
get_current_node (mbrtu_getter_t *getter)
{
	return &(getter->nodes[getter->node_current_i]);
}



static void
goto_next_node (mbrtu_getter_t *getter)
{
	getter->node_current_i = (getter->node_current_i + 1) % getter->num_nodes;

	if (getter->node_current_i == 0 && getter->update_cb)
	{
		getter->update_cb();
	}
}



static int
send_request (mbrtu_getter_t *getter)
{
	mbrtu_gateway_query_t query;

	mbrtu_getter_node_t *node = get_current_node(getter);

	if (HAL_GetTick() - node->last_send_time_ms < node->interval_ms)
	{
		goto_next_node(getter);
		return GETTER_SEND_REQ;
	}

	node->rsp_received = false;

	uint16_t len = mb_makereq_rtu(
			query.req,
			node->id,
			(mb_req_t*) &(node->req));

	query.rsp_callback = response_handler;
	query.rsp_receiver = getter;
	query.timeout = 500;
	query.req_len = len;

	mbrtu_gateway_send_queue(getter->gwy, &query);

	node->last_send_time_ms = HAL_GetTick();

	return GETTER_WAIT_FOR_RSP;
}



static int
wait_for_response (mbrtu_getter_t *getter)
{
	mbrtu_getter_node_t *node = get_current_node(getter);

	if (node->rsp_received)
	{
		node->rsp_received = false;
		goto_next_node(getter);
		
		return GETTER_SEND_REQ;
	}

	return GETTER_WAIT_FOR_RSP;
}



static void
response_handler (void *observer, uint8_t *rsp, uint16_t rsp_len)
{
	mbrtu_getter_t *getter = (mbrtu_getter_t*) observer;
	mbrtu_getter_node_t *node = get_current_node(getter);

	node->rsp_received = true;

	if (!rsp)
	{
		// Handle timeout
		if (node->timeout_count < 255)
			node->timeout_count++;
		LOG(LOG_DEBUG, "Timed out on module %d", getter->node_current_i);
		return;
	}

	if (mb_is_exception(&rsp[1]))
	{
		LOG(LOG_DEBUG, "Exception %02X", rsp[2]);
		return;
	}

	LOG(LOG_DEBUG, "Response on node %d fncode %d", node->id, node->req.fncode);
	LOG_PRINTBUF(LOG_DEBUG, rsp, rsp_len);

	if (rsp[0] == node->id && rsp[1] == node->req.fncode)
	{
		memcpy(node->rsp, rsp, rsp_len);
		node->rsp_len = rsp_len;
	}

	if (node->callback)
		node->callback(node, node->receiver);

	node->timeout_count = 0;
}



#endif /* USE_MBRTU && USE_MBRTU_GETTER && USE_MBRTU_GATEWAY */
