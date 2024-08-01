/*
 * persistent.h
 *
 *  Created on: Feb 1, 2024
 *      Author: Sameen
 */

#ifndef PERSISTENT_H_
#define PERSISTENT_H_

#include "config.h"

#if USE_PERSISTENT
#include <i2c.h>


#ifndef USE_24XX
#define USE_24XX
#endif

#ifndef EEPROM_I2C
#define EEPROM_I2C hi2c1
#endif

/*
 * persistent storage addresses
 *
 * 0   - 1FF	Device Configuration values
 * 400 - 7FF	Device Parameters (Project Specific)
 */

#define PERS_SERIAL_NO 					0x0
#define PERS_FW_VERSION					0x10
#define PERS_HW_VERSION					0x20
#define PERS_SLAVE_ID					0x30

#define PERS_DEV_PARAMS_BASE			0x400



void
persistent_read (uint16_t addr, uint8_t *data, uint16_t count);

void
persistent_write (uint16_t addr, uint8_t *data, uint16_t count);

uint16_t
persistent_read16 (uint16_t addr);

uint32_t
persistent_read32 (uint16_t addr);

void
persistent_write16 (uint16_t addr, uint16_t val);

void
persistent_write32 (uint16_t addr, uint32_t val);

#endif /* USE_PERSISTENT */

#endif /* PERSISTENT_H_ */
