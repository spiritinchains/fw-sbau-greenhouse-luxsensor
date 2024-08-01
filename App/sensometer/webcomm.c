/*
 * comm.c
 *
 *  Created on: Feb 6, 2024
 *      Author: Sameen
 */


#define LOG_SUBSYSTEM "webcomm"

#include "webcomm.h"


#if USE_WEBCOMM

#include <stdio.h>
#include <string.h>

#include <common/log.h>
#include <common/queue.h>
#include <modbus/util.h>
#include <modbus/server.h>
#include <sensometer/m2m.h>
#include <sensometer/mbrtu_gateway.h>
#include <sensometer/mbrtu_slave.h>
#include <json/tiny-json.h>


typedef QUEUE_T(webcomm_pkt_t, WEBCOMM_PKT_QUEUE_SIZE) webcomm_pkt_queue_t;

webcomm_pkt_queue_t pkt_queue = QUEUE_INIT(WEBCOMM_PKT_QUEUE_SIZE);

extern mbrtu_gateway_t gwy_channel;


void webcomm_process_mbascii (webcomm_pkt_t*);
void webcomm_gateway_callback (void*, uint8_t*, uint16_t);
void webcomm_send_modbus(uint16_t, uint8_t *, uint8_t);


void
webcomm_process_packet (webcomm_pkt_t *pkt)
{
	switch (pkt->pid)
	{
	case WEBCOMM_PROT_MBASCII:
		webcomm_process_mbascii(pkt);
		break;
	default:
		break;
	}
}



void
webcomm_process_mbascii (webcomm_pkt_t *pkt)
{
	uint8_t rtu_req[256];
	uint8_t rtu_rsp[256];

	size_t rtu_reqlen;
	size_t rtu_rsplen;

	rtu_reqlen = mbascii_to_rtu((uint8_t*) (pkt->payload), rtu_req, strlen(pkt->payload));

	if (rtu_reqlen == 0)
	{
		// bad ascii packet
		LOG(LOG_DEBUG, "Failed Parsing Modbus ASCII frame");
		return;
	}

	if (rtu_req[0] == slave_id)
	{
		rtu_rsplen = mb_pdu_process(&rtu_req[1], &rtu_rsp[1]);
		rtu_rsp[0] = slave_id;

		webcomm_send_modbus(pkt->tid, rtu_rsp, rtu_rsplen + 3);
	}
	else
	{
		LOG(LOG_DEBUG, "Slave ID mismatch, passing through to gateway");

		QUEUE_PUSH(&pkt_queue, pkt);

		mbrtu_gateway_query_t query = {
				.rsp_receiver = pkt,
				.rsp_callback = webcomm_gateway_callback,
				.timeout = 500,
				.req_len = rtu_reqlen,
		};

		memcpy(query.req, rtu_req, rtu_reqlen);

		mbrtu_gateway_send_now(&gwy_channel, &query);
	}
}



void
webcomm_send_modbus(uint16_t tid, uint8_t *frame, uint8_t frame_len)
{
	webcomm_pkt_t pkt = {
		.serial = DEVICE_SERIAL_ID,
		.tstamp = HAL_GetTick(),
		.tid = tid,
		.pid = 1,
	};

	mbascii_from_rtu((uint8_t *) pkt.payload, frame, frame_len);

	uint8_t m2m_payload[2048];
	int m2m_payload_size = webcomm_emit_json(&pkt, (char *) &m2m_payload[1]);
	m2m_payload[0] = 0x03;

	m2m_send_payload(m2m_payload, m2m_payload_size + 1);
}



void
webcomm_gateway_callback (void *receiver, uint8_t *rsp, uint16_t rsp_len)
{
	if (rsp == NULL)
	{
		LOG(LOG_DEBUG, "Timed out");
		QUEUE_POP(&pkt_queue);
		return;
	}

	webcomm_pkt_t *orig_request = (webcomm_pkt_t*) receiver;
	webcomm_pkt_t pkt;

	strncpy(pkt.serial, orig_request->serial, 16);
	pkt.tstamp = orig_request->tstamp;
	pkt.pid = orig_request->pid;
	pkt.tid = orig_request->tid;

	// crc field does not need to be valid for mbascii conversion
	mbascii_from_rtu((uint8_t*) (pkt.payload), rsp, rsp_len);

	// TODO: update timestamp

	uint8_t m2m_rsp[2048];

	m2m_rsp[0] = 0x03;

	int m2m_rsp_len = webcomm_emit_json(&pkt, (char*) &m2m_rsp[1]) + 1;

	m2m_send_payload(m2m_rsp, m2m_rsp_len);

	QUEUE_POP(&pkt_queue);
}



void
webcomm_parse_json (char *json_str, webcomm_pkt_t *out_pkt)
{
	const char ERR_NOT_ENOUGH_FIELDS[] = "Failed Parsing JSON: more fields than expected";
	const char ERR_REQ_FIELDS_MISSING[] = "Failed Parsing JSON: required fields missing";
	const char ERR_BAD_FIELD_TYPE[] = "Failed Parsing JSON: field \"%s\" has incorrect type";

	json_t pool[10];

	json_t const* parent = json_create(json_str, pool, 10);

	if (!parent)
	{
		LOG(LOG_DEBUG, ERR_NOT_ENOUGH_FIELDS);
		return;
	}

	// extract json fields
	json_t const* field_serial = json_getProperty(parent, "serial");
	json_t const* field_tstamp = json_getProperty(parent, "tstamp");
	json_t const* field_tid = json_getProperty(parent, "tid");
	json_t const* field_pid = json_getProperty(parent, "pid");
	json_t const* field_payload = json_getProperty(parent, "payload");

	// check if fields are valid
	if (!field_serial || !field_tstamp || !field_tid || !field_pid || !field_payload)
	{
		LOG(LOG_DEBUG, ERR_REQ_FIELDS_MISSING);
		return;
	}

	if (json_getType(field_serial) != JSON_TEXT)
	{
		LOG(LOG_DEBUG, ERR_BAD_FIELD_TYPE, "serial");
		return;
	}

	if (json_getType(field_payload) != JSON_TEXT)
	{
		LOG(LOG_DEBUG, ERR_BAD_FIELD_TYPE, "payload");
		return;
	}

	if (json_getType(field_tstamp) != JSON_INTEGER)
	{
		LOG(LOG_DEBUG, ERR_BAD_FIELD_TYPE, "tstamp");
		return;
	}

	if (json_getType(field_tid) != JSON_INTEGER)
	{
		LOG(LOG_DEBUG, ERR_BAD_FIELD_TYPE, "tid");
		return;
	}

	if (json_getType(field_pid) != JSON_INTEGER)
	{
		LOG(LOG_DEBUG, ERR_BAD_FIELD_TYPE, "pid");
		return;
	}

	// assign fields to comm packet structure
	strncpy(out_pkt->serial, json_getValue(field_serial), WEBCOMM_PKT_SERIALNO_SIZE);
	strncpy(out_pkt->payload, json_getValue(field_payload), WEBCOMM_PKT_PAYLOAD_SIZE);
	out_pkt->tstamp = json_getInteger(field_tstamp);
	out_pkt->tid = json_getInteger(field_tid);
	out_pkt->pid = json_getInteger(field_pid);

	return;
}



int
webcomm_emit_json (webcomm_pkt_t *pkt, char *json_str)
{
	const char JSON_FORMAT[] = "{\"serial\":\"%s\","
			"\"tstamp\":%lu,"
			"\"tid\":%lu,"
			"\"pid\":%lu,"
			"\"payload\":\"%s\"}";

	return sprintf(json_str, JSON_FORMAT, pkt->serial, pkt->tstamp, pkt->tid, pkt->pid, pkt->payload);
}


#endif /* USE_WEBCOMM */
