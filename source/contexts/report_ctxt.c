/* ============================================================== */
/*
 * report.c
 *
 * Copyright (c) 2017 David Hammond
 * All Rights Reserved.
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

#include <stdio.h>
#include <inttypes.h>
#include <errno.h>
#include <string.h>

#include "config_ctxt.h"
#include "login_ctxt.h"
#include "report_ctxt.h"

#include "credentials.h"
#include "powermon_calc.h"
#include "powermon_curses.h"
#include "powermon_logger.h"
#include "powermon_time.h"
#include "user_io_fsm.h"


/* Context */
static void init_report_fsm(void);

/* Screen draw */
static void report_screen(void);

#define REPORT_CTXT_STR "report"

io_context_t report_ctxt = {
	.context = report_context,
	.ctxt_str = { 0 },
	.curr_state = user_io_state_init,
	.msg = { 0 }
};


static void report_screen(void)
{
	time_t curr_tm = time(NULL);
	char *timeBuf = ctime(&curr_tm);

	PwrmonTime pwrmonTime;
	getPwrmonElapsedSystemTime(&pwrmonTime);

	erase(); /* Clear the screen */

	printw("\t ========================================================================================= \n");
	printw("\t|                                                                                         |\n");
	printw("\t|                                    Report Menu                                          |\n");
	printw("\t|                                                                                         |\n");
	printw("\t ========================================================================================= \n");
	printw("\t|                         |                  |  Active  |  Avg ON | Avg OFF | Avg |  H/B  |\n");
	printw("\t|    Serial Number        |      nodeIp      | Disable  |  mm:ss  |  mm:ss  | Amp | Alert |\n");
	printw("\t ========================================================================================= \n");
	refresh();
	/*                 1         2         3         4         5         6         7         8         9  */
	/*        1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901 */
	/*          1. 000000399098171-0001 |     10.0.0.8     |    No    |  14:34  |  30:23  | 23  |   *   | */

	consoleReportActiveNodes(); /* In powermon_calc.c */

	printw("\t ========================================================================================= \n");
	printw("\t System Statistics\n");
	printw("\t ========================================================================================= \n");
	consoleReportSystemStatus(); /* In powermon_calc.c */
	printw("\t ========================================================================================= \n");
	printw("\t %s", timeBuf);
	printw("\t Elapsed time: %d days, %02d:%02d:%02d\n\n",
			pwrmonTime.days, pwrmonTime.hours, pwrmonTime.mins, pwrmonTime.secs);
	printw("\t ========================================================================================= \n");
	printw("\t|                                                                                         |\n");
	printw("\t|                                    t - top menu                                         |\n");
	printw("\t|                                    r - refresh                                          |\n");
	printw("\t|                                  1:n - dump node                                        |\n");
	printw("\t|                                    o - logout                                           |\n");
	printw("\t|                                                                                         |\n");
	printw("\t ========================================================================================= \n");
	printw("\t\t: ");
	refresh();
}

static void dump_node_screen(void)
{
	char *endptr;
	uint32_t nodeIndex = strtoimax(report_ctxt.msg.data, &endptr, 10);

	erase(); /* Clear the screen */

	printw("\t ========================================================================================= \n");
	printw("\t|                                                                                         |\n");
	printw("\t|                                     Dump Node                                           |\n");
	printw("\t|                                                                                         |\n");
	printw("\t ========================================================================================= \n");

	consoleReportDumpActiveNode(nodeIndex); /* In powermon_calc.c */

	printw("\t ========================================================================================= \n");
	printw("\t|                                                                                         |\n");
	printw("\t|                                    t - top menu                                         |\n");
	printw("\t|                                    r - refresh                                          |\n");
	printw("\t|                                  1:n - dump node                                        |\n");
	printw("\t|                                    o - logout                                           |\n");
	printw("\t|                                                                                         |\n");
	printw("\t ========================================================================================= \n");
	printw("\t\t: ");
	refresh();
}

static void init_report_fsm(void)
{
	user_io_fsm_register(
			user_io_state_report,
			user_io_input_key_t,
			config_context);
	user_io_fsm_register(
			user_io_state_report,
			user_io_input_key_r,
			report_screen);
	user_io_fsm_register(
			user_io_state_report,
			user_io_input_key_numeric,
			dump_node_screen);
	user_io_fsm_register(
			user_io_state_report,
			user_io_input_key_o,
			login_context);
}

void configure_input(pwrmon_msg_t *msg)
{
	memcpy(&report_ctxt.msg, msg, sizeof(pwrmon_msg_t));
}

void report_context(void)
{
	POWERMON_LOGGER(USER_IO, DEBUG, "report_context\n", 0);

	if (report_ctxt.curr_state == user_io_state_init)
	{
		POWERMON_LOGGER(USER_IO, TRACE, "Context being initialized\n", 0);

		strncpy(report_ctxt.ctxt_str, REPORT_CTXT_STR, MAX_CTXT_STR_LEN);

		init_report_fsm();
		report_ctxt.curr_state = user_io_state_report;
	}

	user_io_ctxt_switch(&report_ctxt);

	erase();
	report_screen();
}

