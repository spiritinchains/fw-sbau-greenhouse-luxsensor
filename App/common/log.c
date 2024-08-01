/*
 * log.c
 *
 *  Created on: Jul 9, 2023
 *      Author: Sameen
 */


#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>

#include <main.h>

#include "log.h"


#ifndef LOG_MSG_SIZE
#define LOG_MSG_SIZE 	256
#endif

#ifndef LOG_OUTPUT_SIZE
#define LOG_OUTPUT_SIZE (LOG_MSG_SIZE + 64)
#endif


#ifdef DEBUG
	static log_level_t log_level_global = LOG_DEBUG;
#else
	static log_level_t log_level_global = LOG_NOTICE;
#endif


const char log_level_str[8][8] = {
		[LOG_FATAL]  = "FATAL  ",
		[LOG_ERROR]  = "ERROR  ",
		[LOG_WARN]   = "WARN   ",
		[LOG_NOTICE] = "NOTICE ",
		[LOG_INFO]   = "INFO   ",
		[LOG_DEBUG]  = "DEBUG  ",
		[LOG_TRACE]  = "TRACE  ",
};



log_level_t
log_getlevel (void)
{
	return log_level_global;
}



void
log_setlevel (log_level_t level)
{
	log_level_global = level;
}



void
_log_msg (log_level_t level, const char* subsys, const char* fmt, ...)
{
	// if level invalid, do nothing
	if (level >= LOG_ALL || level <= LOG_NONE)
		return;

	char msg_buf[LOG_MSG_SIZE];
	char out_buf[LOG_OUTPUT_SIZE];

	va_list args;
	va_start(args, fmt);
	vsnprintf(msg_buf, LOG_MSG_SIZE, fmt, args);
	va_end(args);

	const char log_fmt[] = "[%lu:%02u:%02u:%02u.%03u] %.7s %.20s: %s\r\n";
	
	uint64_t t;
	uint16_t ms;
	uint8_t hh, mm, ss;

	// get timestamp values
	t = HAL_GetTick();

	ms = t % 1000;
	t /= 1000;
	ss = t % 60;
	t /= 60;
	mm = t % 60;
	t /= 60;
	hh = t % 24;
	t /= 24;

	size_t len;

	len = snprintf(out_buf, LOG_OUTPUT_SIZE, log_fmt, (uint32_t)t, hh, mm, ss, ms,
				   &log_level_str[level][0], subsys, msg_buf);

	log_write(out_buf, len);

}



void
_log_printbuf (log_level_t level, const char* subsys, uint8_t* buf, size_t len)
{
	char rowbuf[50];
	int rowidx = 0;
	for (size_t i = 0; i < len; i++)
	{
		int n = snprintf(&rowbuf[rowidx], 4, "%02X ", buf[i]);
		rowidx += n;

		if (i % 16 == 15)
		{
			_log_msg(level, subsys, rowbuf);
			rowidx = 0;
		}
	}

	if (rowidx == 0)
	{
		return;
	}
	_log_msg(level, subsys, rowbuf);
}



__weak void
log_write (const char* buffer, size_t len)
{
	printf(buffer);
}


