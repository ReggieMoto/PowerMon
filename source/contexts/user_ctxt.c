/* ============================================================== */
/*
 * user_ctxt.c
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

#include "login_ctxt.h"
#include "config_ctxt.h"
#include "user_ctxt.h"
#include "add_user_ctxt.h"
#include "edit_user_ctxt.h"

#include "powermon_curses.h"
#include "powermon_logger.h"
#include "user_io_fsm.h"

/* Context */
static void init_user_fsm(void);

/* Screen draw */
static void user_screen(void);

/* FSM actions */

#define USER_CTXT_STR "user"

io_context_t user_ctxt = {
	.context = user_context,
	.ctxt_str = { 0 },
	.curr_state = user_io_state_init
};

static void user_screen(void)
{
	printw("\n\n");
	printw("\t ==========================\n");
	printw("\t|                          |\n");
	printw("\t|        User Menu         |\n");
	printw("\t|                          |\n");
	printw("\t ==========================\n");
	printw("\t|                          |\n");
	printw("\t|      n - new user        |\n");
	printw("\t|      e - edit user       |\n");
	printw("\t|      p - password        |\n");
	printw("\t|   --------------------   |\n");
	printw("\t|      t - top menu        |\n");
	printw("\t|      o - logout          |\n");
	printw("\t|                          |\n");
	printw("\t ==========================\n\n");
	printw("\t\t: ");
	refresh();
}


static void init_user_fsm(void)
{
	user_io_fsm_register(
			user_io_state_user,
			user_io_input_key_n,
			add_user_context);
	user_io_fsm_register(
			user_io_state_user,
			user_io_input_key_e,
			edit_user_context);
	user_io_fsm_register(
			user_io_state_user,
			user_io_input_key_t,
			config_context);
	user_io_fsm_register(
			user_io_state_user,
			user_io_input_key_o,
			login_context);
}

void user_context(void)
{
	POWERMON_LOGGER(USER_IO, DEBUG, "user_context\n", 0);

	if (user_ctxt.curr_state == user_io_state_init)
	{
		POWERMON_LOGGER(USER_IO, TRACE, "Context being initialized\n", 0);

		strncpy(user_ctxt.ctxt_str, USER_CTXT_STR, MAX_CTXT_STR_LEN);

		init_user_fsm();
		user_ctxt.curr_state = user_io_state_user;
	}

	user_io_ctxt_switch(&user_ctxt);

	erase();
	user_screen();
}


