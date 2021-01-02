/* ============================================================== */
/*
 * powermon_logger.c
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

#include <pthread.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>

#include "powermon_logger.h"

static powermon_log_level_e global_log_level;
static powermon_log_thread_e global_thread_logging;

static pthread_mutex_t logger_mutex = PTHREAD_MUTEX_INITIALIZER;

/* =================================
 * set_logger_level
 */
void set_logger_level(powermon_log_level_e new_level)
{
	pthread_mutex_lock(&logger_mutex);
	global_log_level = new_level;
	pthread_mutex_unlock(&logger_mutex);
}

/* =================================
 * get_logger_level
 */
powermon_log_level_e get_logger_level(void)
{
	powermon_log_level_e level;
	pthread_mutex_lock(&logger_mutex);
	level = global_log_level;
	pthread_mutex_unlock(&logger_mutex);
	return level;
}

/* =================================
 * set_logger_thread
 */
void set_logger_thread(powermon_log_thread_e new_thread)
{
	pthread_mutex_lock(&logger_mutex);
	global_thread_logging = new_thread;
	pthread_mutex_unlock(&logger_mutex);
}

/* =================================
 * get_logger_thread
 */
powermon_log_thread_e get_logger_thread(void)
{
	powermon_log_thread_e tid;
	pthread_mutex_lock(&logger_mutex);
	tid = global_thread_logging;
	pthread_mutex_unlock(&logger_mutex);
	return tid;
}

/* =================================
 * get_logger_thread_str
 */
char * get_logger_level_str(powermon_log_level_e log_level)
{
	char *level_str;

	switch(log_level)
	{
	case FATAL:
		level_str = "FATAL\t";
		break;
	case WARN:
		level_str = "WARN\t";
		break;
	case THREAD:
		level_str = "THREAD\t";
		break;
	case INFO:
		level_str = "INFO\t";
		break;
	case DEBUG:
		level_str = "DEBUG\t";
		break;
	case TRACE:
		level_str = "TRACE\t";
		break;
	default:
		level_str = "UNK\t";
		break;
	}

	return level_str;
}

/* =================================
 * get_logger_thread_str
 */
char * get_logger_thread_str(powermon_log_thread_e thread_id)
{
	char *thread_str;

	switch(thread_id)
	{
	case MAIN:
		thread_str = "MAIN\t";
		break;
	case PWRMON:
		thread_str = "PWRMON\t";
		break;
	case USER_IO:
		thread_str = "USR_IO\t";
		break;
	case CONSOLE_IO:
		thread_str = "CNSL_IO\t";
		break;
	case XCONSOLE_IO:
		thread_str = "XCNSL_IO\t";
		break;
	case DEV_IO:
		thread_str = "DEV_IO\t";
		break;
	case DSTORE:
		thread_str = "DSTORE\t";
		break;
	case MSGQ:
		thread_str = "MSGQ\t";
		break;
	case AVAHI:
		thread_str = "AVAHI\t";
		break;
	case CALC:
		thread_str = "CALC\t";
		break;
	default:
		thread_str = "UNK\t";
		break;
	}

	return thread_str;
}

/* =================================
 * powermon_logger
 */

#define STR_BUILDER_LEN 256
static char str_builder[STR_BUILDER_LEN];
#define STR_ENTRY_LEN 1024
static char log_entry[STR_ENTRY_LEN];

void powermon_logger(powermon_log_thread_e thread_id, powermon_log_level_e log_level, char *filename, int line_no, const char *format_str, ...)
{
	powermon_log_level_e level = get_logger_level();
	powermon_log_thread_e tid = get_logger_thread();

	if ((log_level <= level) && (tid != NONE))
	{
		if ((tid == ALL) || (thread_id == tid))
		{
			pthread_mutex_lock(&logger_mutex);

			memset(str_builder, 0, STR_BUILDER_LEN);
			memset(log_entry, 0, STR_ENTRY_LEN);

			/* Get the time */
			/* vsprintf(log_entry, "%s - ", now()); */

			/* Get the thread we are logging from */
			sprintf(log_entry, "%s", get_logger_thread_str(thread_id));

			/* Get the logging level */
			strcat(log_entry, get_logger_level_str(log_level));

			/* Get the filename and the line number */
			sprintf(str_builder, "%s\tline %3d: ", filename, line_no);
			strcat(log_entry, str_builder);

			/* Get the text string containing the log text */
			memset(str_builder, 0, STR_BUILDER_LEN);
			va_list args;
			va_start(args, format_str);
			vsprintf(str_builder, format_str, args);
			strcat(log_entry, str_builder);

			printf("%s", log_entry);

			pthread_mutex_unlock(&logger_mutex);
		}
	}
}

int powermon_logger_init(void)
{
	static char fname[256];
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	memset((void *)fname, 0, 256);
	printf("Powermon (%d) starting up at %d-%d-%d %d:%d:%d\n", getpid(), tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	set_logger_level(DEBUG);
	set_logger_thread(CALC);

	return 0;
}
