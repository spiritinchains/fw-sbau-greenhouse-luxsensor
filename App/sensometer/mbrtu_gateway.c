/*
 * mbrtu_gateway.c
 *
 *  Created on: Feb 19, 2024
 *      Author: Sameen
 */


#define LOG_SUBSYSTEM "mbrtu_gateway"
#define LOG_LEVEL_LOCAL LOG_TRACE

#include "mbrtu_gateway.h"

#if USE_MBRTU && USE_MBRTU_GATEWAY

#include <string.h>

#include <common/log.h>
#include <modbus/util.h>




static mbrtu_gateway_queue_t* get_current_queue(mbrtu_gateway_t*);

static void exec_rsp_handler (mbrtu_gateway_t*, uint8_t*, uint16_t);

static bool is_cooldown_elapsed(mbrtu_gateway_t*);
static bool is_timedout(mbrtu_gateway_t*);

mbrtu_gateway_state_t mbrtu_gateway_sendreq (mbrtu_gateway_t*);
mbrtu_gateway_state_t mbrtu_gateway_recvrsp (mbrtu_gateway_t*);


void
mbrtu_gateway_init (mbrtu_gateway_t *gwy, mbrtu_t *ch)
{
	gwy->mbrtu = ch;

	gwy->time_last_req_sent = 0;
	gwy->time_last_rsp_received = 0;

	gwy->state = GWY_STATE_SENDREQ;

	gwy->queue = (mbrtu_gateway_queue_t) QUEUE_INIT(MBRTU_GATEWAY_QUEUE_MAXSIZE);
	gwy->advance_queue = (mbrtu_gateway_queue_t) QUEUE_INIT(MBRTU_GATEWAY_QUEUE_MAXSIZE);

	gwy->cur_queue = NULL;
	gwy->cur_query = NULL;
}



void
mbrtu_gateway_process (mbrtu_gateway_t *gwy)
{
	switch (gwy->state)
	{
	case GWY_STATE_SENDREQ:
		gwy->state = mbrtu_gateway_sendreq(gwy);
		break;

	case GWY_STATE_RECVRSP:
		gwy->state = mbrtu_gateway_recvrsp(gwy);
		break;

	default:
		gwy->state = GWY_STATE_SENDREQ;
		break;
	}
}



mbrtu_gateway_state_t
mbrtu_gateway_sendreq (mbrtu_gateway_t *gwy)
{
	if (!is_cooldown_elapsed(gwy))
	{
		return GWY_STATE_SENDREQ;
	}

	gwy->cur_queue = get_current_queue(gwy);

	if (gwy->cur_queue)
	{
		gwy->cur_query = &(QUEUE_FRONT(gwy->cur_queue));

		uint8_t *req     = gwy->cur_query->req;
		uint16_t req_len = gwy->cur_query->req_len;

		LOG(LOG_TRACE, "Sending request");
		LOG_PRINTBUF(LOG_TRACE, req, req_len);

		ssize_t len = mbrtu_write(gwy->mbrtu, req, req_len);

		if (len >= 0)
		{
			gwy->time_last_req_sent = HAL_GetTick();
			return GWY_STATE_RECVRSP;
		}
		else
		{
			LOG(LOG_DEBUG, "Failed to send request - retrying");
		}
	}

	return GWY_STATE_SENDREQ;
}



mbrtu_gateway_state_t
mbrtu_gateway_recvrsp (mbrtu_gateway_t *gwy)
{
	uint8_t rsp[MBRTU_GATEWAY_RSP_SIZE];
	ssize_t rsp_len = mbrtu_read(gwy->mbrtu, rsp, MBRTU_GATEWAY_RSP_SIZE);

	if (rsp_len >= 0)
	{
		LOG(LOG_TRACE, "Received %d bytes", rsp_len);
		LOG_PRINTBUF(LOG_TRACE, rsp, rsp_len);

		if (rsp[0] != gwy->cur_query->req[0] || crc16_compute(rsp, rsp_len) != 0)
		{
			LOG(LOG_DEBUG, "Slave ID or CRC Mismatch");

			return GWY_STATE_RECVRSP;
		}
		else
		{
			exec_rsp_handler(gwy, rsp, rsp_len);
			gwy->time_last_rsp_received = HAL_GetTick();

			QUEUE_POP(gwy->cur_queue);
			return GWY_STATE_SENDREQ;
		}
	}
	else if (is_timedout(gwy))
	{
		LOG(LOG_TRACE, "Response Timed Out");

		exec_rsp_handler(gwy, NULL, 0);

		QUEUE_POP(gwy->cur_queue);
		return GWY_STATE_SENDREQ;
	}

	return GWY_STATE_RECVRSP;
}



static void
exec_rsp_handler (mbrtu_gateway_t *gwy, uint8_t *rsp, uint16_t rsp_len)
{
	if (gwy->cur_query->rsp_callback && gwy->cur_query->rsp_receiver)
	{
		gwy->cur_query->rsp_callback(gwy->cur_query->rsp_receiver, rsp, rsp_len);
	}
}



mbrtu_gateway_queue_t*
get_current_queue (mbrtu_gateway_t *gwy)
{
	if (!QUEUE_ISEMPTY(&(gwy->advance_queue)))
	{
		return &(gwy->advance_queue);
	}
	else if (!QUEUE_ISEMPTY(&(gwy->queue)))
	{
		return &(gwy->queue);
	}
	else
	{
		return NULL;
	}
}



bool
is_cooldown_elapsed (mbrtu_gateway_t *gwy)
{
	return ((HAL_GetTick() - gwy->time_last_rsp_received) > MBRTU_GATEWAY_COOLDOWN_MS);
}



bool
is_timedout (mbrtu_gateway_t *gwy)
{
	if (!(gwy->cur_query))
	{
		return true;
	}
	else
	{
		return ((HAL_GetTick() - gwy->time_last_req_sent) > gwy->cur_query->timeout);
	}
}



int
mbrtu_gateway_send_queue (mbrtu_gateway_t *gwy, mbrtu_gateway_query_t *query)
{
	QUEUE_PUSH(&(gwy->queue), query) else return -1;
	return 0;
}



int
mbrtu_gateway_send_now (mbrtu_gateway_t *gwy, mbrtu_gateway_query_t *query)
{
	QUEUE_PUSH(&(gwy->advance_queue), query) else return -1;
	return 0;
}


#endif /* USE_MBRTU && USE_MBRTU_GATEWAY */
