/*
 * log.h
 *
 *  Created on: Aug 13, 2023
 *      Author: Sameen
 */

#ifndef COMMON_LOG_H_
#define COMMON_LOG_H_


#include <stdint.h>
#include <stddef.h>


typedef enum
{
	LOG_NONE   = 0,			// No logs
	LOG_FATAL  = 1,			// Unrecoverable errors
	LOG_ERROR  = 2,			// General recoverable errors
	LOG_WARN   = 3,			// Events that can lead to an error
	LOG_NOTICE = 4,			// Notable non-error events
	LOG_INFO   = 5,			// User-driven events or regular system events
	LOG_DEBUG  = 6,			// Information required only for debugging
	LOG_TRACE  = 7,			// Only used for tracking bugs, removed in prod
	LOG_ALL    = 8			// All logs

} log_level_t;



log_level_t 
log_getlevel (void);

void 
log_setlevel (log_level_t level);

void 
_log_msg (log_level_t level, const char* subsys, const char* fmt, ...);

void 
_log_printbuf (log_level_t level, const char* subsys, uint8_t* buf, size_t len);

void 
log_write (const char* buffer, size_t len);



#ifndef LOG_SUBSYSTEM
	#define LOG_SUBSYSTEM "???"
#endif


#ifdef LOG_LEVEL_LOCAL
	#define LOG_LEVEL LOG_LEVEL_LOCAL
#else
	#define LOG_LEVEL log_getlevel()
#endif


#define LOG(level, msg, ... ) \
	do { 														\
		if (level <= LOG_LEVEL) {								\
			_log_msg(level, LOG_SUBSYSTEM, msg, ##__VA_ARGS__); \
		}														\
	} while (0)


#define LOG_PRINTBUF(level, buf, len) \
	do { 														\
		if (level <= LOG_LEVEL) {								\
			_log_printbuf(level, LOG_SUBSYSTEM, buf, len); 		\
		}														\
	} while (0)



#endif /* COMMON_LOG_H_ */
