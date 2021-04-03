/* ============================================================== */
/*
 * login.c
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

#include "credentials.h"
#include "powermon_curses.h"
#include "powermon_logger.h"
#include "user_io_fsm.h"

/* Context */
static void init_login_fsm(void);

/* Screen draw */
static void print_login_password(void);
static void print_login_username(void);
static void login_screen(void);

/* FSM actions */
static void reset_login(void);
static void login(void);
static void get_passwd(void);
static void verify_credentials(void);
static void login_invalid(void);

#define LOGIN_CTXT_STR "login"

io_context_t login_ctxt = {
	.context = login_context,
	.ctxt_str = { 0 },
	.curr_state = user_io_state_init
};

static void reset_login(void)
{
	reset_credentials();

	erase();
	login_screen();
	print_login_username();

	login_ctxt.curr_state = user_io_state_login;
}

static void login(void)
{
	POWERMON_LOGGER(USER_IO, DEBUG, "Action: user_io_action_login\n", 0);

	reset_login();
}

static void get_passwd(void)
{
	POWERMON_LOGGER(USER_IO, DEBUG, "Action: user_io_action_password\n", 0);

	print_login_password();
}

static void verify_credentials(void)
{
	POWERMON_LOGGER(USER_IO, DEBUG, "Action: user_io_action_password\n", 0);

	set_credentials_invalid();
	validate_credentials();
}

static void login_invalid(void)
{
	POWERMON_LOGGER(USER_IO, DEBUG, "Action: user_io_action_login_invalid\n", 0);

	reset_login();
}

static void login_screen(void)
{
	printw("\t _______\n");
	printw("\t/       \\\n");
	printw("\t$$$$$$$  | ______   __   __   __   ______    ______\n");
	printw("\t$$ |__$$ |/      \\ /  | /  | /  | /      \\  /      \\\n");
	printw("\t$$    $$//$$$$$$  |$$ | $$ | $$ |/$$$$$$  |/$$$$$$  |\n");
	printw("\t$$$$$$$/ $$ |  $$ |$$ | $$ | $$ |$$    $$ |$$ |  $$/\n");
	printw("\t$$ |     $$ \\__$$ |$$ \\_$$ \\_$$ |$$$$$$$$/ $$ |\n");
	printw("\t$$ |     $$    $$/ $$   $$   $$/ $$       |$$ |\n");
	printw("\t$$/       $$$$$$/   $$$$$/$$$$/   $$$$$$$/ $$/\n");
	printw("\n\n\n");
	printw("\t       __       __\n");
	printw("\t      /  \\     /  |\n");
	printw("\t      $$  \\   /$$ |  ______   _______\n");
	printw("\t      $$$  \\ /$$$ | /      \\ /       \\\n");
	printw("\t      $$$$  /$$$$ |/$$$$$$  |$$$$$$$  |\n");
	printw("\t      $$ $$ $$/$$ |$$ |  $$ |$$ |  $$ |\n");
	printw("\t      $$ |$$$/ $$ |$$ \\__$$ |$$ |  $$ |\n");
	printw("\t      $$ | $/  $$ |$$    $$/ $$ |  $$ |\n");
	printw("\t      $$/      $$/  $$$$$$/  $$/   $$/\n");
	printw("\n\n");
	printw("\t\t ======================\n");
	printw("\t\t|    PowerMon Login    |\n");
	printw("\t\t ======================\n");
	refresh();
}

static void print_login_username(void)
{
	printw("\t\tusername: ");
	refresh();
}

static void print_login_password(void)
{
	printw("\t\tpassword: ");
	refresh();
}

static void init_login_fsm(void)
{
	user_io_fsm_register(
			user_io_state_login,
			user_io_input_login_start,
			login);
	user_io_fsm_register(
			user_io_state_login,
			user_io_input_login_name,
			get_passwd);
	user_io_fsm_register(
			user_io_state_login,
			user_io_input_login_passw,
			verify_credentials);
	user_io_fsm_register(
			user_io_state_login,
			user_io_input_login_valid,
			config_context);
	user_io_fsm_register(
			user_io_state_login,
			user_io_input_login_invalid,
			login_invalid);
}

void login_context(void)
{
	POWERMON_LOGGER(USER_IO, DEBUG, "login_context\n", 0);

	if (login_ctxt.curr_state == user_io_state_init)
	{
		POWERMON_LOGGER(USER_IO, TRACE, "Context being initialized\n", 0);

		strncpy(login_ctxt.ctxt_str, LOGIN_CTXT_STR, MAX_CTXT_STR_LEN);

		init_login_fsm();
		login_ctxt.curr_state = user_io_state_login;
	}

	user_io_ctxt_switch(&login_ctxt);

	login();
}

