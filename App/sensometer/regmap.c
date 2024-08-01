/*
 * regmap.c
 *
 *  Created on: Jan 31, 2024
 *      Author: Sameen
 */


#include "mbrtu_slave.h"

#if USE_MB_REGMAP || USE_MBRTU_SLAVE || USE_MBTCP_SLAVE

#if USE_MBRTU_SLAVE
#include "mbrtu_slave.h"
#endif

#include <common/log.h>
#include <modbus/regs.h>

extern uint16_t luxval_high, luxval_low;


int
reg_read_4x (uint16_t addr, uint16_t *val)
{
	if (addr == 0x110)
	{
		// Lux High
		*val = luxval_high;
		return 0;
	}
	else if (addr == 0x111)
	{
		// Lux Low
		*val = luxval_low;
		return 0;
	}
	else
	{
		*val = 0xFFFF;
		return 0;
	}
}



#endif /* USE_MB_REGMAP || USE_MBRTU_SLAVE || USE_MBTCP_SLAVE */
