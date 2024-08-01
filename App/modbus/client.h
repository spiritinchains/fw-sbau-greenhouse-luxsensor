/*
 * client.h
 *
 *  Created on: Oct 31, 2023
 *      Author: Sameen
 */

#ifndef MODBUS_CLIENT_H_
#define MODBUS_CLIENT_H_

#include "util.h"

#include <stdbool.h>
#include <sys/types.h>

typedef struct _mb_req mb_req_t;


struct _mb_req
{
	uint8_t fncode;
	uint16_t start;
	uint16_t count;

	uint16_t write_start;		// ONLY for FC 23
	uint16_t write_count;		// ONLY for FC 23

	uint8_t *data;
	uint8_t data_len;

	uint16_t sub_fncode;
};


ssize_t mb_makereq (uint8_t *pdu, mb_req_t *req);
ssize_t mb_makereq_rtu (uint8_t *req_buf, uint8_t id, mb_req_t *req);
ssize_t mb_makereq_tcp (uint8_t *req_buf, uint16_t tid, uint8_t id, mb_req_t *req);

uint16_t mb_pdu_getreg (uint8_t *pdu, uint8_t index);
int mb_pdu_getregs (uint8_t *pdu, uint16_t *regs, size_t n);

bool mb_is_exception (uint8_t *pdu);


#endif /* MODBUS_CLIENT_H_ */
