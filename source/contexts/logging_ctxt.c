/* ============================================================== */
/*
 * logging_ctxt.c
 *
 * Copyright (c) 2021 David Hammond
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

#include "config_ctxt.h"
#include "logging_ctxt.h"

#include "powermon_curses.h"
#include "powermon_logger.h"
#include "user_io.h"
#include "user_io_fsm.h"

/* Context */
static void init_logging_fsm(void);

/* Screen draw */
static void logging_screen(void);

/* FSM actions */

#define LOGGING_CTXT_STR "logging"

io_context_t logging_ctxt = {
	.context = logging_context,
	.ctxt_str = { 0 },
	.curr_state = user_io_state_init,
	.msg = { 0 }
};

static void logging_screen(void)
{
	erase(); /* Clear the screen */

	char *level = get_logger_level_str(get_logger_level());
	char *module = get_logger_module_str(get_logger_module());

	printw("\n\n");
	printw("\t ==================================\n");
	printw("\t|                                  |\n");
	printw("\t|           Logging Menu           |\n");
	printw("\t|                                  |\n");
	printw("\t|     Current logging config       |\n");
	printw("\t|       Module: %s         |\n", module);
	printw("\t|        Level: %s             |\n", level);
	printw("\t|                                  |\n");
	printw("\t ==================================\n");
	printw("\t|                                  |\n");
	printw("\t|   n - None          0 - OFF      |\n");
	printw("\t|   a - All           1 - FATAL    |\n");
	printw("\t|   c - CALC          2 - WARN     |\n");
	printw("\t|   d - DSTORE        3 - THREAD   |\n");
	printw("\t|   e - DEV_IO        4 - INFO     |\n");
	printw("\t|   i - CONSOLE_IO    5 - DEBUG    |\n");
	printw("\t|   m - MAIN          6 - TRACE    |\n");
	printw("\t|   p - PWRMON                     |\n");
	printw("\t|   q - MSGQ                       |\n");
	printw("\t|   u - USER_IO                    |\n");
	printw("\t|   v - AVAHI                      |\n");
	printw("\t|                                  |\n");
	printw("\t|   ----------------------------   |\n");
	printw("\t|                                  |\n");
	printw("\t|    Enter a module and a level    |\n");
	printw("\t|    Example: a6 for All/TRACE     |\n");
	printw("\t|                                  |\n");
	printw("\t|          t - top menu            |\n");
	printw("\t|                                  |\n");
	printw("\t ==================================\n");
	printw("\t\t: ");
	refresh();
}

static void logging_change(void)
{
	if (strlen(logging_ctxt.msg.data) == 2) {

		char module = logging_ctxt.msg.data[0];
		char level  = logging_ctxt.msg.data[1];

		POWERMON_LOGGER(USER_IO, INFO, "Logging change: Module: %c Level: %c\n",
				logging_ctxt.msg.data[0], logging_ctxt.msg.data[1]);

		set_logger_level(get_logger_level_from_char(level));
		set_logger_module(get_logger_module_from_char(module));
	}

	logging_context();
}

static void init_logging_fsm(void)
{
	POWERMON_LOGGER(USER_IO, TRACE, "init_config_fsm\n", 0);

	user_io_fsm_register(
			user_io_state_logging,
			user_io_input_key_t,
			config_context);

	user_io_fsm_register(
			user_io_state_logging,
			user_io_input_logging_chg,
			logging_change);
}

void cfg_logging_input(pwrmon_msg_t *msg)
{
	memset(&logging_ctxt.msg, 0, sizeof(pwrmon_msg_t));
	memcpy(&logging_ctxt.msg, msg, sizeof(pwrmon_msg_t));
}

void logging_context(void)
{
	POWERMON_LOGGER(USER_IO, TRACE, "logging_context\n", 0);

	if (logging_ctxt.curr_state == user_io_state_init)
	{
		strncpy(logging_ctxt.ctxt_str, LOGGING_CTXT_STR, MAX_CTXT_STR_LEN);

		init_logging_fsm();
		logging_ctxt.curr_state = user_io_state_logging;
	}

	user_io_ctxt_switch(&logging_ctxt);

	erase();
	logging_screen();
}

