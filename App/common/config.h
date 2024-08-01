/*
 * config.h
 *
 *  Created on: Mar 12, 2024
 *      Author: Sameen
 */

#ifndef COMMON_CONFIG_H_
#define COMMON_CONFIG_H_


/* Load User configuration header if defined */
#ifdef USER_CONFIG_HEADER
#include USER_CONFIG_HEADER
#endif

#ifndef PRINTF_UART
#define PRINTF_UART huart2
#endif

#endif /* COMMON_CONFIG_H_ */
