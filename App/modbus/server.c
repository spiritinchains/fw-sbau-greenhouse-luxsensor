/*
 * server.c
 *
 *  Created on: Aug 28, 2023
 *      Author: Sameen
 */


#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "util.h"
#include "server.h"
#include "regs.h"


size_t mb_read_coils(uint8_t*, uint8_t*);
size_t mb_read_dinps(uint8_t*, uint8_t*);
size_t mb_read_regs(uint8_t*, uint8_t*);
size_t mb_read_holding(uint8_t*, uint8_t*);
size_t mb_write_coil(uint8_t*, uint8_t*);
size_t mb_write_holding(uint8_t*, uint8_t*);
size_t mb_write_multi_coil(uint8_t*, uint8_t*);
size_t mb_write_multi_reg(uint8_t*, uint8_t*);



size_t
mb_pdu_process(uint8_t *req, uint8_t *rsp)
{
	mb_fn_code_t fn_code = req[0];
	rsp[0] = fn_code;
	uint8_t rsplen = 0;

	switch (fn_code)
	{

	case MB_FN_READ_COILS:
	{
		rsplen = mb_read_coils(req, rsp);
		break;
	}

	case MB_FN_READ_DISCRETE_INPUTS:
	{
		rsplen = mb_read_dinps(req, rsp);
		break;
	}

	case MB_FN_READ_INPUT_REGISTERS:
	{
		rsplen = mb_read_regs(req, rsp);
		break;
	}

	case MB_FN_READ_HOLDING_REGISTERS:
	{
		rsplen = mb_read_holding(req, rsp);
		break;
	}

	case MB_FN_WRITE_SINGLE_COIL:
	{
		rsplen = mb_write_coil(req, rsp);
		break;
	}

	case MB_FN_WRITE_SINGLE_REGISTER:
	{
		rsplen = mb_write_holding(req, rsp);
		break;
	}

	case MB_FN_WRITE_MULTI_COIL:
	{
		rsplen = mb_write_multi_coil(req, rsp);
		break;
	}

	case MB_FN_WRITE_MULTI_REGISTERS:
	{
		rsplen = mb_write_multi_reg(req, rsp);
		break;
	}

	default:
	{
		// unsupported function
		rsplen = mb_makeexc(fn_code, MB_EXC_ILLEGAL_FUNCTION, rsp);
		break;
	}

	}

	return rsplen;
}



size_t
mb_read_coils(uint8_t *req, uint8_t *rsp)
{
	uint16_t start_addr = (req[1] << 8) | req[2];
	uint16_t num_coils = (req[3] << 8) | req[4];

	if (num_coils < 1 || num_coils > 0x7D0)
		return mb_makeexc(req[0], MB_EXC_ILLEGAL_DATA_VALUE, rsp);

	for (int i = 0; i < num_coils; i++)
	{
		bool bit;

		int err = reg_read_0x(start_addr + i, &bit);
		if (err < 0)
		{
			return mb_makeexc(req[0], MB_EXC_ILLEGAL_DATA_ADDRESS, rsp);
		}

		rsp[(i / 8) + 2] |= (bit & 1) << (i % 8);
	}

	rsp[1] = (num_coils + 7) / 8;

	return 2 + rsp[1];
}



size_t
mb_read_dinps(uint8_t *req, uint8_t *rsp)
{
	uint16_t start_addr = (req[1] << 8) | req[2];
	uint16_t num_coils = (req[3] << 8) | req[4];

	if (num_coils < 1 || num_coils > 0x7D0)
		return mb_makeexc(req[0], MB_EXC_ILLEGAL_DATA_VALUE, rsp);

	for (int i = 0; i < num_coils; i++)
	{
		bool bit;

		int err = reg_read_1x(start_addr + i, &bit);
		if (err < 0)
		{
			return mb_makeexc(req[0], MB_EXC_ILLEGAL_DATA_ADDRESS, rsp);
		}

		rsp[(i / 8) + 2] |= (bit & 1) << (i % 8);
	}

	rsp[1] = (num_coils + 7) / 8;

	return 2 + rsp[1];
}



size_t
mb_read_regs(uint8_t *req, uint8_t *rsp)
{
	uint16_t start_addr = (req[1] << 8) | req[2];
	uint16_t num_regs = (req[3] << 8) | req[4];

	if (num_regs < 1 || num_regs > 0x7D)
		return mb_makeexc(req[0], MB_EXC_ILLEGAL_DATA_VALUE, rsp);

	for (int i = 0; i < num_regs; i++)
	{
		uint16_t reg;

		int err = reg_read_3x(start_addr + i, &reg);
		if (err < 0)
		{
			return mb_makeexc(req[0], MB_EXC_ILLEGAL_DATA_ADDRESS, rsp);
		}

		rsp[2 * i + 2] = (reg >> 8) & 0xFF;
		rsp[2 * i + 3] = reg & 0xFF;
	}

	rsp[1] = num_regs * 2;

	return 2 + rsp[1];
}



size_t
mb_read_holding(uint8_t *req, uint8_t *rsp)
{
	uint16_t start_addr = (req[1] << 8) | req[2];
	uint16_t num_regs = (req[3] << 8) | req[4];

	if (num_regs < 1 || num_regs > 0x7D)
		return mb_makeexc(req[0], MB_EXC_ILLEGAL_DATA_VALUE, rsp);

	for (int i = 0; i < num_regs; i++)
	{
		uint16_t reg;

		int err = reg_read_4x(start_addr + i, &reg);
		if (err < 0)
		{
			return mb_makeexc(req[0], MB_EXC_ILLEGAL_DATA_ADDRESS, rsp);
		}

		rsp[2 * i + 2] = (reg >> 8) & 0xFF;
		rsp[2 * i + 3] = reg & 0xFF;
	}

	rsp[1] = num_regs * 2;

	return 2 + rsp[1];
}



size_t
mb_write_coil(uint8_t *req, uint8_t *rsp)
{
	uint16_t addr = (req[1] << 8) | req[2];
	uint16_t val = (req[3] << 8) | req[4];

	bool bval;

	if (val == 0x0000)
	{
		bval = false;
	}
	else if (val == 0xff00)
	{
		bval = true;
	}
	else
	{
		return mb_makeexc(req[0], MB_EXC_ILLEGAL_DATA_VALUE, rsp);
	}

	int err = reg_write_0x(addr, bval);
	if (err < 0)
	{
		return mb_makeexc(req[0], MB_EXC_ILLEGAL_DATA_ADDRESS, rsp);
	}

	memcpy(rsp, req, 5);
	return 5;
}



size_t
mb_write_holding(uint8_t *req, uint8_t *rsp)
{
	uint16_t addr = (req[1] << 8) | req[2];
	uint16_t val = (req[3] << 8) | req[4];

	int err = reg_write_4x(addr, val);
	if (err < 0)
	{
		return mb_makeexc(req[0], MB_EXC_ILLEGAL_DATA_ADDRESS, rsp);
	}

	memcpy(rsp, req, 5);
	return 5;
}



size_t
mb_write_multi_coil(uint8_t *req, uint8_t *rsp)
{
	uint16_t start_addr = (req[1] << 8) | req[2];
	uint16_t num_coils = (req[3] << 8) | req[4];

	uint8_t num_bytes = req[5];

	if (num_coils < 1 && num_coils > 0x7D0 && num_bytes != (num_coils + 7) / 8)
		return mb_makeexc(req[0], MB_EXC_ILLEGAL_DATA_VALUE, rsp);

	// verify validity of addresses
	for (int i = 0; i < num_coils; i++)
	{
		bool bit;
		int err = reg_read_0x(start_addr + i, &bit);
		if (err < 0)
		{
			return mb_makeexc(req[0], MB_EXC_ILLEGAL_DATA_ADDRESS, rsp);
		}
	}

	// write addresses if valid
	for (int i = 0; i < num_coils; i++)
	{
		bool bit = (req[6 + (i / 8)] >> (i % 8)) & 1;

		int err = reg_write_0x(start_addr + i, bit);
		if (err < 0)
		{
			return mb_makeexc(req[0], MB_EXC_SLAVE_DEVICE_FAILURE, rsp);
		}
	}

	memcpy(rsp, req, 5);
	return 5;
}



size_t
mb_write_multi_reg(uint8_t* req, uint8_t* rsp)
{
	uint16_t start_addr = (req[1] << 8) | req[2];
	uint16_t num_regs = (req[3] << 8) | req[4];

	uint8_t num_bytes = req[5];

	if (num_regs < 1 && num_regs > 0x7B && num_bytes != 2 * num_regs)
	{
		return mb_makeexc(req[0], MB_EXC_ILLEGAL_DATA_VALUE, rsp);
	}

	// verify validity of addresses
	for (int i = 0; i < num_regs; i++)
	{
		uint16_t reg;
		int err = reg_read_4x(start_addr + i, &reg);
		if (err < 0)
		{
			return mb_makeexc(req[0], MB_EXC_ILLEGAL_DATA_ADDRESS, rsp);
		}
	}

	// write addresses if valid
	for (int i = 0; i < num_regs; i++)
	{
		uint16_t reg = (req[6 + 2 * i] << 8) | req[7 + 2 * i];

		int err = reg_write_4x(start_addr + i, reg);
		if (err < 0)
		{
			return mb_makeexc(req[0], MB_EXC_SLAVE_DEVICE_FAILURE, rsp);
		}
	}

	memcpy(rsp, req, 5);
	return 5;
}



size_t
mb_makeexc(mb_fn_code_t fn_code, mb_exc_t exc, uint8_t *rsp)
{
	rsp[0] = fn_code + 0x80;
	rsp[1] = (uint8_t)exc;
	return 2;
}



