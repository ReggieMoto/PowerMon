/* ============================================================== */
/*
 * edit_user_ctxt.c
 *
 * Copyright (c) 2020 David Hammond
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
#include "edit_user_ctxt.h"

#include "powermon_curses.h"
#include "powermon_logger.h"
#include "user_io_fsm.h"

/* Context */
static void init_edit_user_fsm(void);

/* Screen draw */
static void edit_user_screen(void);

/* FSM actions */

#define EDIT_USER_CTXT_STR "edit_user"

io_context_t edit_user_ctxt = {
	.context = edit_user_context,
	.ctxt_str = { 0 },
	.curr_state = user_io_state_init
};

static void edit_user_screen(void)
{
	printw("\n\n");
	printw("\t ==========================\n");
	printw("\t|                          |\n");
	printw("\t|      Edit User Menu      |\n");
	printw("\t|                          |\n");
	printw("\t ==========================\n");
	printw("\t|                          |\n");
	printw("\t|     n - name             |\n");
	printw("\t|     e - email address    |\n");
	printw("\t|     d - phone            |\n");
	printw("\t|     p - password         |\n");
	printw("\t|   --------------------   |\n");
	printw("\t|     u - user             |\n");
	printw("\t|     t - top menu         |\n");
	printw("\t|     o - logout           |\n");
	printw("\t|                          |\n");
	printw("\t ==========================\n\n");
	printw("\t\t: ");
	refresh();
}

static void init_edit_user_fsm(void)
{
	user_io_fsm_register(
			user_io_state_modify_user,
			user_io_input_key_u,
			user_context);
	user_io_fsm_register(
			user_io_state_modify_user,
			user_io_input_key_t,
			config_context);
	user_io_fsm_register(
			user_io_state_modify_user,
			user_io_input_key_o,
			login_context);
}

void edit_user_context(void)
{
	POWERMON_LOGGER(USER_IO, DEBUG, "edit_user_context\n", 0);

	if (edit_user_ctxt.curr_state == user_io_state_init)
	{
		POWERMON_LOGGER(USER_IO, TRACE, "Context being initialized\n", 0);

		strncpy(edit_user_ctxt.ctxt_str, EDIT_USER_CTXT_STR, MAX_CTXT_STR_LEN);

		init_edit_user_fsm();
		edit_user_ctxt.curr_state = user_io_state_modify_user;
	}

	user_io_ctxt_switch(&edit_user_ctxt);

	erase();
	edit_user_screen();
}


