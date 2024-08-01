/*
 * regs.h
 *
 *  Created on: Aug 28, 2023
 *      Author: Sameen
 */

#ifndef MODBUS_REGS_H_
#define MODBUS_REGS_H_


#include <stdint.h>
#include <stdbool.h>


int reg_read_0x(uint16_t addr, bool *val);
int reg_read_1x(uint16_t addr, bool *val);
int reg_read_3x(uint16_t addr, uint16_t *val);
int reg_read_4x(uint16_t addr, uint16_t *val);
int reg_write_0x(uint16_t addr, bool val);
int reg_write_4x(uint16_t addr, uint16_t val);


#endif /* MODBUS_REGS_H_ */
