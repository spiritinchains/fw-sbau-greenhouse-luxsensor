/*
 * client.c
 *
 *  Created on: Oct 31, 2023
 *      Author: Sameen
 */


#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

#include "client.h"
#include "util.h"


static ssize_t
mkpdu_read_multi (uint8_t *pdu, mb_req_t *req_desc);

static ssize_t
mkpdu_write_single (uint8_t *pdu, mb_req_t *req_desc);

static ssize_t
mkpdu_write_multi (uint8_t *pdu, mb_req_t *req_desc);

static ssize_t
mkpdu_read_write_multi (uint8_t *pdu, mb_req_t *req_desc);

static ssize_t
mkpdu_read_write_file (uint8_t *pdu, mb_req_t *req_desc);

static ssize_t
mkpdu_mask_write (uint8_t *pdu, mb_req_t *req_desc);

static ssize_t
mkpdu_read_fifo (uint8_t *pdu, mb_req_t *req_desc);

static ssize_t
mkpdu_single (uint8_t *pdu, mb_req_t *req_desc);

static ssize_t
mkpdu_diag (uint8_t *pdu, mb_req_t *req_desc);




ssize_t
mb_makereq_rtu (uint8_t *req_buf, uint8_t id, mb_req_t *req)
{
	req_buf[0] = id;

	ssize_t pdulen = mb_makereq(&req_buf[1], req);

	if (pdulen < 0)
	{
		return pdulen;
	}

	uint16_t crc = crc16_compute(req_buf, pdulen + 1);

	req_buf[pdulen + 1] = crc & 0xFF;
	req_buf[pdulen + 2] = (crc >> 8) & 0xFF;

	return pdulen + 3;
}



ssize_t
mb_makereq_tcp (uint8_t *req_buf, uint16_t tid, uint8_t id, mb_req_t *req)
{
	// Transaction ID
	req_buf[0] = (tid >> 8) & 0xFF;
	req_buf[1] = tid & 0xFF;

	// Protocol ID
	req_buf[2] = 0x00;
	req_buf[3] = 0x00;

	// Unit ID
	req_buf[6] = id;

	ssize_t pdulen = mb_makereq(&req_buf[7], req);

	// Length
	uint16_t len = pdulen + 1;
	req_buf[4] = (len >> 8) & 0xFF;
	req_buf[5] = len & 0xFF;

	return pdulen + 7;
}


ssize_t
mb_makereq (uint8_t *pdu, mb_req_t *req_desc)
{
	uint8_t fncode = req_desc->fncode;

	ssize_t ret;

	switch (fncode)
	{
	case MB_FN_READ_COILS:
	case MB_FN_READ_DISCRETE_INPUTS:
	case MB_FN_READ_HOLDING_REGISTERS:
	case MB_FN_READ_INPUT_REGISTERS:
		ret = mkpdu_read_multi(pdu, req_desc);
		break;

	case MB_FN_WRITE_SINGLE_COIL:
	case MB_FN_WRITE_SINGLE_REGISTER:
		ret = mkpdu_write_single(pdu, req_desc);
		break;

	case MB_FN_WRITE_MULTI_COIL:
	case MB_FN_WRITE_MULTI_REGISTERS:
		ret = mkpdu_write_multi(pdu, req_desc);
		break;

	case MB_FN_READ_WRITE_MULTI_REGS:
		ret = mkpdu_read_write_multi(pdu, req_desc);
		break;

	case MB_FN_READ_FILE_RECORD:
	case MB_FN_WRITE_FILE_RECORD:
		ret = mkpdu_read_write_file(pdu, req_desc);
		break;

	case MB_FN_MASK_WRITE_REGISTER:
		ret = mkpdu_mask_write(pdu, req_desc);
		break;

	case MB_FN_READ_FIFO_QUEUE:
		ret = mkpdu_read_fifo(pdu, req_desc);
		break;

	case MB_FN_READ_EXCEPTION_STATUS:
	case MB_FN_GET_COMM_EVENT_CTR:
	case MB_FN_GET_COMM_EVENT_LOG:
	case MB_FN_REPORT_SERVER_ID:
		ret = mkpdu_single(pdu, req_desc);
		break;

	case MB_FN_DIAGNOSTICS:
		ret = mkpdu_diag(pdu, req_desc);
		break;

	default:
		ret = -1;
		break;
	}

	return ret;
}



static ssize_t
mkpdu_read_multi (uint8_t *pdu, mb_req_t *req_desc)
{
	pdu[0] = req_desc->fncode;

	pdu[1] = (req_desc->start >> 8) & 0xFF;
	pdu[2] = req_desc->start & 0xFF;

	pdu[3] = (req_desc->count >> 8) & 0xFF;
	pdu[4] = req_desc->count & 0xFF;

	return 5;
}



static ssize_t
mkpdu_write_single (uint8_t *pdu, mb_req_t *req_desc)
{
	pdu[0] = req_desc->fncode;

	pdu[1] = (req_desc->start >> 8) & 0xFF;
	pdu[2] = req_desc->start & 0xFF;

	pdu[3] = req_desc->data[0];
	pdu[4] = req_desc->data[1];

	return 5;
}



static ssize_t
mkpdu_write_multi (uint8_t *pdu, mb_req_t *req_desc)
{
	pdu[0] = req_desc->fncode;

	pdu[1] = (req_desc->start >> 8) & 0xFF;
	pdu[2] = req_desc->start & 0xFF;

	pdu[3] = (req_desc->count >> 8) & 0xFF;
	pdu[4] = req_desc->count & 0xFF;

	pdu[5] = req_desc->data_len;

	for (int i = 0; i < req_desc->data_len; i++)
	{
		pdu[6 + i] = req_desc->data[i];
	}

	return 6 + req_desc->data_len;
}



static ssize_t
mkpdu_read_write_multi (uint8_t *pdu, mb_req_t *req_desc)
{
	pdu[0] = req_desc->fncode;

	pdu[1] = (req_desc->start >> 8) & 0xFF;
	pdu[2] = req_desc->start & 0xFF;

	pdu[3] = (req_desc->count >> 8) & 0xFF;
	pdu[4] = req_desc->count & 0xFF;

	pdu[5] = (req_desc->write_start >> 8) & 0xFF;
	pdu[6] = req_desc->write_start & 0xFF;

	pdu[7] = (req_desc->write_count >> 8) & 0xFF;
	pdu[8] = req_desc->write_count & 0xFF;

	pdu[9] = req_desc->data_len;

	for (int i = 0; i < req_desc->data_len; i++)
	{
		pdu[10 + i] = req_desc->data[i];
	}

	return 10 + req_desc->data_len;
}



static ssize_t
mkpdu_read_write_file (uint8_t *pdu, mb_req_t *req_desc)
{
	// TODO: unimplemented

	return -1;
}



static ssize_t
mkpdu_mask_write (uint8_t *pdu, mb_req_t *req_desc)
{
	// TODO: unimplemented

	return -1;
}



static ssize_t
mkpdu_read_fifo (uint8_t *pdu, mb_req_t *req_desc)
{
	// TODO: unimplemented

	return -1;
}



static ssize_t
mkpdu_single (uint8_t *pdu, mb_req_t *req_desc)
{
	pdu[0] = req_desc->fncode;

	return 1;
}



static ssize_t
mkpdu_diag (uint8_t *pdu, mb_req_t *req_desc)
{
	// TODO: unimplemented

	return -1;
}



int
mb_pdu_getregs (uint8_t *pdu, uint16_t *regs, size_t n)
{
	// NOTE: function code is assumed to be 03 or 04

	uint8_t byte_count = pdu[1];

	if (byte_count % 2 != 0)
	{
		// byte count can not be odd
		return -1;
	}

	for (int i = 0; i < (byte_count / 2) && i < n; i++)
	{
		regs[i] = 0;
		regs[i] |= pdu[2 + (2 * i)];
		regs[i] <<= 8;
		regs[i] |= pdu[3 + (2 * i)];
	}

	return (byte_count / 2);
}



int
mb_pdu_getcoils (uint8_t *pdu)
{
	// TODO: unimplemented
	return 0;
}



bool
mb_is_exception (uint8_t *pdu)
{
	return (pdu[0] > 0x80);
}

