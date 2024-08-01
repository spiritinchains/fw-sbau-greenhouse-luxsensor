/*
 * persistent.c
 *
 *  Created on: Feb 1, 2024
 *      Author: Sameen
 */


#include "persistent.h"

#if USE_PERSISTENT

#ifdef USE_24XX
#include <24xx.h>
#endif


void
persistent_read (uint16_t addr, uint8_t *data, uint16_t count)
{
#ifdef USE_24XX
	eeprom_24xx_read(&EEPROM_I2C, addr, data, count);
#endif
	return;
}



void
persistent_write (uint16_t addr, uint8_t *data, uint16_t count)
{
#ifdef USE_24XX
	eeprom_24xx_write(&EEPROM_I2C, addr, data, count);
#endif
	return;
}



uint16_t
persistent_read16 (uint16_t addr)
{
	uint8_t tmp_data[2];
	persistent_read(addr, tmp_data, 2);

	uint16_t res = (tmp_data[0] << 8) | tmp_data[1];

	return res;
}



uint32_t
persistent_read32 (uint16_t addr)
{
	uint8_t tmp_data[4];
	persistent_read(addr, tmp_data, 4);

	uint32_t res = (tmp_data[0] << 24) | (tmp_data[1] << 16) | (tmp_data[2] << 8) | tmp_data[3];

	return res;
}



void
persistent_write16 (uint16_t addr, uint16_t val)
{
	uint8_t tmp_data[2];

	tmp_data[0] = (val >> 8) & 0xFF;
	tmp_data[1] = val & 0xFF;

	persistent_write(addr, tmp_data, 2);
}



void
persistent_write32 (uint16_t addr, uint32_t val)
{
	uint8_t tmp_data[4];

	tmp_data[0] = (val >> 24) & 0xFF;
	tmp_data[1] = (val >> 16) & 0xFF;
	tmp_data[2] = (val >> 8) & 0xFF;
	tmp_data[3] = val & 0xFF;

	persistent_write(addr, tmp_data, 4);
}


#endif /* USE_PERSISTENT */
