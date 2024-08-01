/*
 * comm.h
 *
 *  Created on: Feb 6, 2024
 *      Author: Sameen
 */

#ifndef SENSOMETER_COMM_H_
#define SENSOMETER_COMM_H_

#include "config.h"

#if USE_WEBCOMM
#include <stdint.h>


typedef struct _webcomm_pkt webcomm_pkt_t;

struct _webcomm_pkt
{
	uint32_t tstamp;
	uint32_t tid;
	uint32_t pid;

	char serial[WEBCOMM_PKT_SERIALNO_SIZE];
	char payload[WEBCOMM_PKT_PAYLOAD_SIZE];
};


enum webcomm_prot
{
	WEBCOMM_PROT_MBASCII = 1
};


void
webcomm_parse_json (char *json_str, webcomm_pkt_t *out_pkt);

int
webcomm_emit_json (webcomm_pkt_t *pkt, char *json_str);

void
webcomm_process_packet (webcomm_pkt_t *pkt);

#endif /* USE_WEBCOMM */

#endif /* SENSOMETER_COMM_H_ */
