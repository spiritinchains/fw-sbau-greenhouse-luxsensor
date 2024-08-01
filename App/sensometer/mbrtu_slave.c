/*
 * mbrtu_slave.c
 *
 *  Created on: Oct 22, 2023
 *      Author: Sameen
 */


#define LOG_LEVEL_LOCAL LOG_TRACE
#define LOG_SUBSYSTEM "mbrtu_slave"


#include "mbrtu_slave.h"

#if USE_MBRTU && USE_MBRTU_SLAVE

#include <common/log.h>
#include <modbus/server.h>
#include <modbus/util.h>

#include <sys/types.h>


uint8_t slave_id = MBRTU_SLAVE_DEFAULT_ID;


void
mbrtu_slave_init (mbrtu_slave_t *mbs, mbrtu_t *mbrtu)
{
	mbs->base = mbrtu;
	mbs->id = &slave_id;
}



void
mbrtu_slave_process (mbrtu_slave_t *mbs)
{
	ssize_t reqlen;
	reqlen = mbrtu_read(mbs->base, mbs->req, 256);

	if (reqlen < 6)
	{
		return;
	}

	LOG_PRINTBUF(LOG_TRACE, mbs->req, reqlen);

	uint8_t id = *(mbs->id);
	uint8_t *req = mbs->req;

	if (req[0] != id)
	{
		return;
	}

	if (crc16_compute(&(req[0]), reqlen) != 0)
	{
		return;
	}

	uint8_t rsp[256] = {0};
	uint8_t rsplen = mb_pdu_process(&req[1], &rsp[1]);

	rsp[0] = id;

	uint16_t crc = crc16_compute(rsp, rsplen + 1);
	rsp[rsplen + 1] = crc & 0xFF;
	rsp[rsplen + 2] = (crc >> 8) & 0xFF;

	mbrtu_write(mbs->base, rsp, rsplen + 3);

}


#endif /* USE_MBRTU && USE_MBRTU_SLAVE */
