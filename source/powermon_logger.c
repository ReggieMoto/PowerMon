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
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include "common.h"
#include "powermon_curses.h"
#include "powermon_logger.h"

static powermon_log_level_e global_log_level;
static powermon_log_module_e global_module_logging;

static pthread_mutex_t logger_mutex = PTHREAD_MUTEX_INITIALIZER;

static FILE *logfd;
static unsigned int filesize;

#define LOG_FNAME_MAX 256
#define LOG_STARTUP_STRING "PowerMon (%d) starting up on %d_%02d%02d at %02d:%02d:%02d\n"
#define LOG_FNAME_STRING "PowerMon_%d_%d.log"

#define STR_BUILDER_LEN 256
#define STR_ENTRY_LEN 1024

static int logger_open_logfile(void);
static int logger_close_logfile(void);
static int logger_log_entry(char *log_entry);

/*
 * Open a logfile
 */
static int logger_open_logfile(void)
{
	char fqname[LOG_FNAME_MAX];
	char fname[LOG_FNAME_MAX];
	int status = 0;
	time_t t = time(NULL);

	memset((void *)fqname, 0, LOG_FNAME_MAX);
	memset((void *)fname, 0, LOG_FNAME_MAX);

	snprintf(fqname, LOG_FNAME_MAX-1, POWERMON_LOG_DIR);
	int remaining = LOG_FNAME_MAX - strlen(fqname);
	snprintf(fname, LOG_FNAME_MAX-1, LOG_FNAME_STRING, getpid(), (int)t);
	strncat(fqname, fname, remaining-1);

	logfd = fopen(fqname, "w");
	filesize = 0;

	if (logfd == NULL)
	{
		status = -1;
	}

	return status;
}

static int logger_close_logfile(void)
{
	int status = 0;

	if (logfd != NULL)
	{
		status = fclose(logfd);
	}

	return status;
}

#define MAX_LOGGER_FILESIZE (5*1024*1024) /* 5MB filesize */

static int logger_log_entry(char *log_entry)
{
	int status = 0;
	char log_str[STR_ENTRY_LEN];

	if ((filesize += strlen(log_entry)) >= MAX_LOGGER_FILESIZE)
	{
		status = logger_close_logfile();
		if (status == 0)
		{
			status = logger_open_logfile();
		}
	}

	if ((status == 0) && (logfd != NULL))
	{
		struct timeval read_time;
		gettimeofday(&read_time, NULL);
		long long int current_time =
				(long long int)((read_time.tv_sec * 1000000) + read_time.tv_usec);

		memset(log_str, 0, STR_ENTRY_LEN);
		snprintf(log_str, STR_ENTRY_LEN, "%lld: ", current_time);
		unsigned int remaining = STR_ENTRY_LEN - strlen(log_str);
		strncat(log_str, log_entry, remaining-1);

		status = fwrite(log_str, strlen(log_str), 1, logfd);
		fdatasync(fileno(logfd));
	}

	if (status == 0)
	{
		printw("Error writing to log file\n");
		refresh();
	}

	return status;
}

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
	return (level);
}

/* =================================
 * get_logger_level_from_char
 */
powermon_log_level_e get_logger_level_from_char(char c_level)
{
	powermon_log_level_e level;

	if ((c_level >= '0') && (c_level <= '6')) {

		switch(c_level)
		{
		case '0':
			level = OFF;
			break;
		case '1':
			level = FATAL;
			break;
		case '2':
			level = WARN;
			break;
		case '3':
			level = THREAD;
			break;
		case '4':
			level = INFO;
			break;
		case '5':
			level = DEBUG;
			break;
		case '6':
			level = TRACE;
			break;
		default:
			level = INFO;
			break;
		}
	} else {
		level = INFO;
	}

	return (level);
}

/* =================================
 * set_logger_module
 */
void set_logger_module(powermon_log_module_e new_module)
{
	pthread_mutex_lock(&logger_mutex);
	global_module_logging = new_module;
	pthread_mutex_unlock(&logger_mutex);
}

/* =================================
 * get_logger_module
 */
powermon_log_module_e get_logger_module(void)
{
	powermon_log_module_e tid;
	pthread_mutex_lock(&logger_mutex);
	tid = global_module_logging;
	pthread_mutex_unlock(&logger_mutex);
	return (tid);
}

/* =================================
 * get_logger_module_from_char
 */
powermon_log_module_e get_logger_module_from_char(char c_module)
{
	powermon_log_module_e module;

	if ((c_module >= 'a') && (c_module <= 'z')) {
		switch(c_module)
		{
		case 'a':
			module = ALL;
			break;
		case 'c':
			module = CALC;
			break;
		case 'd':
			module = DSTORE;
			break;
		case 'e':
			module = DEV_IO;
			break;
		case 'i':
			module = CONSOLE_IO;
			break;
		case 'm':
			module = MAIN;
			break;
		case 'n':
			module = NONE;
			break;
		case 'p':
			module = PWRMON;
			break;
		case 'q':
			module = MSGQ;
			break;
		case 'u':
			module = USER_IO;
			break;
		case 'v':
			module = AVAHI;
			break;
		default:
			module = ALL;
			break;
		}
	} else {
		module = ALL;
	}

	return (module);
}


/* =================================
 * get_logger_module_str
 */
char * get_logger_level_str(powermon_log_level_e log_level)
{
	char *level_str;

	switch(log_level)
	{
	case FATAL:
		level_str = "FATAL";
		break;
	case WARN:
		level_str = "WARN";
		break;
	case THREAD:
		level_str = "THREAD";
		break;
	case INFO:
		level_str = "INFO";
		break;
	case DEBUG:
		level_str = "DEBUG";
		break;
	case TRACE:
		level_str = "TRACE";
		break;
	default:
		level_str = "UNK";
		break;
	}

	return (level_str);
}

/* =================================
 * get_logger_module_str
 */
char * get_logger_module_str(powermon_log_module_e module_id)
{
	char *module_str;

	switch(module_id)
	{
	case MAIN:
		module_str = "MAIN";
		break;
	case PWRMON:
		module_str = "PWRMON";
		break;
	case USER_IO:
		module_str = "USR_IO";
		break;
	case CONSOLE_IO:
		module_str = "CNSL_IO";
		break;
	case DEV_IO:
		module_str = "DEV_IO";
		break;
	case DSTORE:
		module_str = "DSTORE";
		break;
	case MSGQ:
		module_str = "MSGQ";
		break;
	case AVAHI:
		module_str = "AVAHI";
		break;
	case CALC:
		module_str = "CALC";
		break;
	case ALL:
		module_str = "ALL";
		break;
	default:
		module_str = "UNK";
		break;
	}

	return (module_str);
}

/* =================================
 * powermon_logger
 */
void powermon_logger(powermon_log_module_e module_id, powermon_log_level_e log_level, char *filename, int line_no, const char *format_str, ...)
{
	powermon_log_level_e level = get_logger_level();
	powermon_log_module_e module = get_logger_module();

	if ((log_level <= INFO) ||
		((level == DEBUG) && (log_level <= level)) ||
		((level == TRACE) && (log_level <= level)))
	{
		if ((module == ALL) || (module_id == module))
		{
			pthread_mutex_lock(&logger_mutex);

			char str_builder[STR_BUILDER_LEN];
			char log_entry[STR_ENTRY_LEN];

			memset(str_builder, 0, STR_BUILDER_LEN);
			memset(log_entry, 0, STR_ENTRY_LEN);

			/* Get the time */
			/* vsprintf(log_entry, "%s - ", now()); */

			/* Get the thread we are logging from */
			sprintf(log_entry, "%s\t", get_logger_module_str(module_id));

			/* Get the logging level */
			strcat(log_entry, get_logger_level_str(log_level));
			strcat(log_entry, "\t");

			/* Get the filename and the line number */
			sprintf(str_builder, "%s\tline %3d: ", filename, line_no);
			strcat(log_entry, str_builder);

			/* Get the text string containing the log text */
			memset(str_builder, 0, STR_BUILDER_LEN);
			va_list args;
			va_start(args, format_str);
			vsprintf(str_builder, format_str, args);
			strcat(log_entry, str_builder);

			logger_log_entry(log_entry);

			pthread_mutex_unlock(&logger_mutex);
		}
	}
}

int powermon_logger_init(void)
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	int status = logger_open_logfile();

	if (status >= 0)
	{
		printw(LOG_STARTUP_STRING, getpid(), tm.tm_year + 1900, tm.tm_mon + 1,
				tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
		refresh();

		set_logger_level(INFO);
		set_logger_module(ALL);
	}
	else
	{
		printw("Unable to open logfile.");
		refresh();
	}

	return (status);
}

void powermon_logger_shutdown(void)
{
	logger_close_logfile();
}
