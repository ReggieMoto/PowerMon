/* ============================================================== */
/*
 * config_ctxt.c
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

#include "config_ctxt.h"
#include "login_ctxt.h"
#include "report_ctxt.h"
#include "user_ctxt.h"

#include "powermon_curses.h"
#include "powermon_logger.h"
#include "user_io.h"
#include "user_io_fsm.h"

/* Context */
static void init_config_fsm(void);

/* Screen draw */
static void config_screen(void);

/* FSM actions */
static void goto_config_logging(void);
static void goto_config_dev_disc(void);
static void goto_config_network(void);

#define CONFIG_CTXT_STR "config"

io_context_t config_ctxt = {
	.context = config_context,
	.ctxt_str = { 0 },
	.curr_state = user_io_state_init
};


static void config_screen(void)
{
	printw("\n\n");
	printw("\t ==========================\n");
	printw("\t|                          |\n");
	printw("\t|      PowerMon Menu       |\n");
	printw("\t|                          |\n");
	printw("\t ==========================\n");
	printw("\t|                          |\n");
	printw("\t|   u - users              |\n");
	printw("\t|   l - logging            |\n");
	printw("\t|   d - device discovery   |\n");
	printw("\t|   c - connectivity       |\n");
	printw("\t|   r - report             |\n");
	printw("\t|   --------------------   |\n");
	printw("\t|   o - logout             |\n");
	printw("\t|                          |\n");
	printw("\t ==========================\n\n");
	printw("\t\t: ");
	refresh();
}

static void goto_config_logging(void)
{
	config_context();
}

static void goto_config_dev_disc(void)
{
	config_context();
}

static void goto_config_network(void)
{
	config_context();
}

static void goto_exit(void)
{
	user_io_send_exit_msg();
}


static void init_config_fsm(void)
{
	POWERMON_LOGGER(USER_IO, TRACE, "init_config_fsm\n", 0);

	user_io_fsm_register(
			user_io_state_main,
			user_io_input_key_u,
			user_context);
	user_io_fsm_register(
			user_io_state_main,
			user_io_input_key_l,
			goto_config_logging);
	user_io_fsm_register(
			user_io_state_main,
			user_io_input_key_d,
			goto_config_dev_disc);
	user_io_fsm_register(
			user_io_state_main,
			user_io_input_key_c,
			goto_config_network);
	user_io_fsm_register(
			user_io_state_main,
			user_io_input_key_r,
			report_context);
	user_io_fsm_register(
			user_io_state_main,
			user_io_input_key_o,
			login_context);
	user_io_fsm_register(
			user_io_state_main,
			user_io_input_key_x,
			goto_exit);
}

void config_context(void)
{
	POWERMON_LOGGER(USER_IO, TRACE, "config_context\n", 0);

	if (config_ctxt.curr_state == user_io_state_init)
	{
		strncpy(config_ctxt.ctxt_str, CONFIG_CTXT_STR, MAX_CTXT_STR_LEN);

		init_config_fsm();
		config_ctxt.curr_state = user_io_state_main;
	}

	user_io_ctxt_switch(&config_ctxt);

	erase();
	config_screen();
}

