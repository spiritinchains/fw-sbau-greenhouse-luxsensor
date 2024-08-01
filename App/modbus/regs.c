/*
 * regs.c
 *
 *  Created on: Aug 28, 2023
 *      Author: Sameen
 */


#include "main.h"

#include "regs.h"


__weak int
reg_read_0x (uint16_t addr, bool *val)
{
	return -1;
}



__weak int
reg_read_1x (uint16_t addr, bool *val)
{
	return -1;
}



__weak int
reg_read_3x (uint16_t addr, uint16_t *val)
{
	return -1;
}



__weak int
reg_read_4x (uint16_t addr, uint16_t *val)
{
	return -1;
}



__weak int
reg_write_0x (uint16_t addr, bool val)
{
	return -1;
}



__weak int
reg_write_4x (uint16_t addr, uint16_t val)
{
	return -1;
}


