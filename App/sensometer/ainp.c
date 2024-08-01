/*
 * ainp.c
 *
 *  Created on: Feb 1, 2024
 *      Author: Sameen
 */

#define LOG_SUBSYSTEM "ainp"

#include "ainp.h"

#if USE_AINP

#include <stdbool.h>

#include <adc.h>
#include <dma.h>

#include <common/log.h>


// TODO: make these configurable from flash memory
uint16_t ainp_gain_factor = 3333;
uint16_t ainp_resistance = 160;

uint16_t ainp_modes[AINP_NUM_INPUTS];

static uint16_t adc_raw[AINP_NUM_INPUTS];

static float get_pin_voltage (uint16_t);
static void adc_dma_start (ADC_HandleTypeDef*, uint32_t*, int);
static void adc_dma_stop (ADC_HandleTypeDef*);



void
ainp_init (ADC_HandleTypeDef *hadc)
{
	adc_dma_start(hadc, (uint32_t *) adc_raw, AINP_NUM_INPUTS);
}



uint16_t
ainp_get (uint16_t index)
{
	if (index >= AINP_NUM_INPUTS)
		return 0;

	float v_pin = get_pin_voltage(adc_raw[index]);

	uint16_t result;

	if (ainp_modes[index] == AINP_MODE_VOLTAGE)
	{
		result = (uint16_t) (v_pin * ainp_gain_factor);
	}
	else if (ainp_modes[index] == AINP_MODE_CURRENT)
	{
		result = (uint16_t) (v_pin * 1000 / ainp_resistance);
	}

	return result;
}



static void
adc_dma_start (ADC_HandleTypeDef *adc, uint32_t *values, int num_values)
{
	HAL_ADC_Start_DMA(adc, values, num_values);
}



static void
adc_dma_stop (ADC_HandleTypeDef *adc)
{
	HAL_ADC_Stop_DMA(adc);
}



static float
get_pin_voltage (uint16_t raw)
{
	return (raw * 3.3) / AINP_ADC_MAX;
}

#endif /* USE_AINP */
