/*
 * config.h
 *
 *  Created on: Mar 2, 2024
 *      Author: jawad
 */

#ifndef SENSOMETER_CONFIG_H_
#define SENSOMETER_CONFIG_H_


/* Load User configuration header if defined */
#ifdef USER_CONFIG_HEADER
#include USER_CONFIG_HEADER
#endif


/* Included Modules */
#ifndef USE_AINP
#define USE_AINP 1
#endif

#ifndef USE_AOUT
#define USE_AOUT 1
#endif

#ifndef USE_M2M
#define USE_M2M 1
#endif

#ifndef USE_MB_REGMAP
#define USE_MB_REGMAP 1
#endif

#ifndef USE_RS485
#define USE_RS485 1
#endif

#ifndef USE_MBRTU
#define USE_MBRTU 1
#endif

#ifndef USE_MBRTU_GATEWAY
#define USE_MBRTU_GATEWAY 1
#endif

#ifndef USE_MBRTU_GETTER
#define USE_MBRTU_GETTER 1
#endif

#ifndef USE_MBRTU_SLAVE
#define USE_MBRTU_SLAVE 1
#endif

#ifndef USE_WEBCOMM
#define USE_WEBCOMM 1
#endif


/* analog input settings */
#ifndef AINP_NUM_INPUTS
#define AINP_NUM_INPUTS 0
#endif

#ifndef AINP_ADC_MAX
#define AINP_ADC_MAX 4095
#endif


/* analog output settings */
#ifndef AOUT_NUM_OUTPUTS
#define AOUT_NUM_OUTPUTS 0
#endif

#ifndef AOUT_PWM_FREQ_KHZ
#define AOUT_PWM_FREQ_KHZ 10
#endif


/* digital input settings */
#ifndef DINP_NUM_INPUTS
#define DINP_NUM_INPUTS 0
#endif


/* digital output settings */
#ifndef DOUT_NUM_OUTPUTS
#define DOUT_NUM_OUTPUTS 0
#endif


/* m2m settings */
#ifndef M2M_BUF_SIZE
#define M2M_BUF_SIZE 2048
#endif

#ifndef M2M_USE_LEGACY
#define M2M_USE_LEGACY 0
#endif


/* rs485 settings */
#ifndef RS485_NUM_IFACES
#define RS485_NUM_IFACES 1
#endif


/* mbrtu settings */
#ifndef MBRTU_BUF_SIZE
#define MBRTU_BUF_SIZE 256
#endif

#ifndef MBRTU_NUM_IFACES
#define MBRTU_NUM_IFACES 1
#endif


/* mbrtu gateway settings */
#ifndef MBRTU_GATEWAY_REQ_SIZE
#define MBRTU_GATEWAY_REQ_SIZE 256
#endif

#ifndef MBRTU_GATEWAY_RSP_SIZE
#define MBRTU_GATEWAY_RSP_SIZE 256
#endif

#ifndef MBRTU_GATEWAY_QUEUE_MAXSIZE
#define MBRTU_GATEWAY_QUEUE_MAXSIZE 16
#endif

#ifndef MBRTU_GATEWAY_COOLDOWN_MS
#define MBRTU_GATEWAY_COOLDOWN_MS 500
#endif


/* mbrtu slave settings */
#ifndef MBRTU_SLAVE_DEFAULT_ID
#define MBRTU_SLAVE_DEFAULT_ID 1
#endif


/* webcomm settings */
#ifndef WEBCOMM_PKT_SERIALNO_SIZE
#define WEBCOMM_PKT_SERIALNO_SIZE 16
#endif

#ifndef WEBCOMM_PKT_PAYLOAD_SIZE
#define WEBCOMM_PKT_PAYLOAD_SIZE 1024
#endif

#ifndef WEBCOMM_PKT_QUEUE_SIZE
#define WEBCOMM_PKT_QUEUE_SIZE 4
#endif


#endif /* SENSOMETER_CONFIG_H_ */
