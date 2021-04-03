/* ============================================================== */
/*
 * powermon_logger.h
 *
 * Copyright (c) 2017 David Hammond
 * All Rights Reserved.
 *
 * Logging by severity level and component/thread.
 */
/* ============================================================== */
/*
 * NOTICE:  All information contained herein is and remains the
 * property of David Hammond. The intellectual and technical
 * concepts contained herein are proprietary to David Hammond
 * and may be covered by U.S.and Foreign Patents, patents in
 * process, and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this
 * material is strictly forbidden unless prior written permission
 * is obtained David Hammond.
 */
/* ============================================================== */

#ifndef __POWERMON_LOGGER_H__
#define __POWERMON_LOGGER_H__

#include <stdio.h>
#include <string.h>

typedef enum {
	powermon_log_level_first = 0,
	OFF = powermon_log_level_first,
	FATAL,
	WARN,
	THREAD,
	INFO,
	DEBUG,
	TRACE,
	powermon_log_level_last = TRACE,
	powermon_log_level_count
} powermon_log_level_e;

typedef enum {
	powermon_log_module_first = 0,
	NONE = powermon_log_module_first,
	MAIN,
	PWRMON,
	USER_IO,
	CONSOLE_IO,
	DEV_IO,
	DSTORE,
	MSGQ,
	AVAHI,
	CALC,
	ALL,
	powermon_log_module_last = ALL,
	powermon_log_module_count
} powermon_log_module_e;

int powermon_logger_init(void);
void powermon_logger_shutdown(void);
void powermon_logger(powermon_log_module_e thread_id, powermon_log_level_e log_level, char *filename, int line_no, const char *format_str, ...);
void set_logger_level(powermon_log_level_e new_level);
powermon_log_level_e get_logger_level(void);
char * get_logger_level_str(powermon_log_level_e log_level);
powermon_log_level_e get_logger_level_from_char(char c_level);
void set_logger_module(powermon_log_module_e new_module);
powermon_log_module_e get_logger_module(void);
char * get_logger_module_str(powermon_log_module_e log_module);
powermon_log_module_e get_logger_module_from_char(char c_module);

#define POWERMON_LOGGER(TID,LVL,STR, ARGS...) powermon_logger(TID,LVL,__FILE__,__LINE__,STR,ARGS)

#endif /* __POWERMON_LOGGER_H__ */
