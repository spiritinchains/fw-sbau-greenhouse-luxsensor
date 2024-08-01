/*
 * server.h
 *
 *  Created on: Aug 28, 2023
 *      Author: Sameen
 */

#ifndef MODBUS_SERVER_H_
#define MODBUS_SERVER_H_


#include <stddef.h>

#include "util.h"


size_t mb_pdu_process(uint8_t*, uint8_t*);
size_t mb_makeexc(mb_fn_code_t, mb_exc_t, uint8_t*);


#endif /* MODBUS_SERVER_H_ */
