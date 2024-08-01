/*
 * ainp.h
 *
 *  Created on: May 30, 2024
 *      Author: sameen
 */

#ifndef SENSOMETER_AINP_H_
#define SENSOMETER_AINP_H_

#include "config.h"

#if USE_AINP
#include <stdint.h>

#include <adc.h>


extern uint16_t ainp_gain_factor;
extern uint16_t ainp_resistance;
extern uint16_t ainp_modes[];


enum ainp_mode {
	AINP_MODE_VOLTAGE = 0,
	AINP_MODE_CURRENT = 1
};


void ainp_init (ADC_HandleTypeDef *hadc);

uint16_t ainp_get (uint16_t index);

#endif /* USE_AINP */

#endif /* SENSOMETER_AINP_H_ */
