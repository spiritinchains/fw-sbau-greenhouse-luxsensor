/*
 * util.h
 *
 *  Created on: Aug 27, 2023
 *      Author: Sameen
 */

#ifndef MODBUS_UTIL_H_
#define MODBUS_UTIL_H_

#include <stdint.h>
#include <stddef.h>


typedef enum modbus_fn_codes
{
	MB_FN_READ_COILS 				= 0x01,
	MB_FN_READ_DISCRETE_INPUTS 		= 0x02,
	MB_FN_READ_HOLDING_REGISTERS 	= 0x03,
	MB_FN_READ_INPUT_REGISTERS 		= 0x04,
	MB_FN_WRITE_SINGLE_COIL 		= 0x05,
	MB_FN_WRITE_SINGLE_REGISTER 	= 0x06,
	MB_FN_READ_EXCEPTION_STATUS 	= 0x07,
	MB_FN_DIAGNOSTICS 				= 0x08,
	MB_FN_GET_COMM_EVENT_CTR 		= 0x0b,
	MB_FN_GET_COMM_EVENT_LOG 		= 0x0c,
	MB_FN_WRITE_MULTI_COIL 			= 0x0f,
	MB_FN_WRITE_MULTI_REGISTERS 	= 0x10,
	MB_FN_REPORT_SERVER_ID 			= 0x11,
	MB_FN_READ_FILE_RECORD 			= 0x14,
	MB_FN_WRITE_FILE_RECORD 		= 0x15,
	MB_FN_MASK_WRITE_REGISTER 		= 0x16,
	MB_FN_READ_WRITE_MULTI_REGS		= 0x17,
	MB_FN_READ_FIFO_QUEUE 			= 0x18
} mb_fn_code_t;


typedef enum modbus_exceptions
{
	MB_EXC_ILLEGAL_FUNCTION 		= 0x01,
	MB_EXC_ILLEGAL_DATA_ADDRESS 	= 0x02,
	MB_EXC_ILLEGAL_DATA_VALUE 		= 0x03,
	MB_EXC_SLAVE_DEVICE_FAILURE 	= 0x04,
	MB_EXC_ACKNOWLEDGE 				= 0x05,
	MB_EXC_SLAVE_DEVICE_BUSY 		= 0x06,
	MB_EXC_MEMORY_PARITY_ERROR 		= 0x08,
	MB_EXC_GATEWAY_PATH_UNAVAILABLE = 0x0a,
	MB_EXC_GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND = 0x0b
} mb_exc_t;


uint16_t
crc16_compute(uint8_t* data, size_t size);

size_t
mbascii_from_rtu (uint8_t *ascii_msg, uint8_t *rtu_msg, size_t rtu_msg_size);

size_t
mbascii_to_rtu (uint8_t *ascii_msg, uint8_t *rtu_msg, size_t ascii_msg_size);

#endif /* MODBUS_UTIL_H_ */
