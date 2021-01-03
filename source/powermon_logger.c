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
#include "powermon_logger.h"

static powermon_log_level_e global_log_level;
static powermon_log_thread_e global_thread_logging;

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

	printf("Opening logfile: %s\n", fqname);

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
		printf("Closing logfile\n");
		status = fclose(logfd);
	}

	return status;
}

#define MAX_LOGGER_FILESIZE (8*1024) /* 8k filesize */

static int logger_log_entry(char *log_entry)
{
	int status = 0;
	char log_str[STR_ENTRY_LEN];

	if ((strlen(log_entry) + filesize) >= MAX_LOGGER_FILESIZE)
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
		printf("Error writing to log file\n");

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
		thread_str = "MAIN\t\t";
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
		thread_str = "MSGQ\t\t";
		break;
	case AVAHI:
		thread_str = "AVAHI\t\t";
		break;
	case CALC:
		thread_str = "CALC\t\t";
		break;
	default:
		thread_str = "UNK  \t";
		break;
	}

	return thread_str;
}

/* =================================
 * powermon_logger
 */
void powermon_logger(powermon_log_thread_e thread_id, powermon_log_level_e log_level, char *filename, int line_no, const char *format_str, ...)
{
	powermon_log_level_e level = get_logger_level();
	powermon_log_thread_e tid = get_logger_thread();

	if ((tid != NONE) &&
		((log_level <= INFO) ||
		((level == DEBUG) && (log_level <= level)) ||
		((level == TRACE) && (log_level <= level))))
	{
		if ((tid == ALL) || (thread_id == tid))
		{
			pthread_mutex_lock(&logger_mutex);

			char str_builder[STR_BUILDER_LEN];
			char log_entry[STR_ENTRY_LEN];

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
		printf(LOG_STARTUP_STRING, getpid(), tm.tm_year + 1900, tm.tm_mon + 1,
				tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

		set_logger_level(INFO);
		set_logger_thread(ALL);
	}
	else
	{
		printf("Unable to open logfile.");
	}

	return status;
}

void powermon_logger_shutdown(void)
{
	logger_close_logfile();
}
