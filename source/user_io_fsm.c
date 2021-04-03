/* ============================================================== */
/*
 * user_io_fsm.c
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

#include <string.h>

#include "credentials.h"
#include "powermon_logger.h"
#include "report_ctxt.h"
#include "user_io_fsm.h"

/* Initialized when starting up the user_io thread */
static io_context_t *current_context = NULL;
static user_io_action fsm_action[user_io_state_count][user_io_input_count];


static const char *user_io_fsm_state_str[] = {
	"user_io_state_init",
	"user_io_state_login",
	"user_io_state_main",
		"user_io_state_user",
			"user_io_state_new_user",
			"user_io_state_modify_user",
			"user_io_state_delete_user",
		"user_io_state_logging",
			"user_io_state_log_level",
			"user_io_state_log_comp",
		"user_io_state_discovery",
		"user_io_state_connectivity",
		"user_io_state_cfg_user",
};

static const char * get_user_io_fsm_state_str(user_io_state_e state)
{
	return (user_io_fsm_state_str[state]);
}

static const char *user_io_fsm_input_str[] = {
	"user_io_input_start",
	"user_io_input_login_name",
	"user_io_input_login_passw",
	"user_io_input_login_valid",
	"user_io_input_login_invalid",
	"user_io_input_key_b",
	"user_io_input_key_c",
	"user_io_input_key_d",
	"user_io_input_key_e",
	"user_io_input_key_l",
	"user_io_input_key_m",
	"user_io_input_key_n",
	"user_io_input_key_o",
	"user_io_input_key_p",
	"user_io_input_key_r",
	"user_io_input_key_t",
	"user_io_input_key_u",
	"user_io_input_key_x",
	"user_io_input_key_1",
	"user_io_input_key_2",
	"user_io_input_key_3",
	"user_io_input_key_4",
	"user_io_input_key_5",
	"user_io_input_key_6",
	"user_io_input_key_7",
	"user_io_input_key_8",
	"user_io_input_key_9",
	"user_io_input_unhandled"
};

static const char * get_user_io_fsm_input_str(user_io_input_e input)
{
	return (user_io_fsm_input_str[input]);
}


void user_io_fsm(user_io_input_e input)
{
	user_io_state_e state = current_context->curr_state;

	POWERMON_LOGGER(USER_IO, DEBUG, "State:  %s\n", get_user_io_fsm_state_str(state));
	POWERMON_LOGGER(USER_IO, DEBUG, "Input:  %s\n", get_user_io_fsm_input_str(input));

	(fsm_action[state][input])();
}

void user_io_fsm_process_string(pwrmon_msg_t *msg)
{
	POWERMON_LOGGER(USER_IO, TRACE, "current_context: %s\n",
			current_context->ctxt_str);
	POWERMON_LOGGER(USER_IO, TRACE, "current_context->curr_state: %s\n",
			get_user_io_fsm_state_str(current_context->curr_state));

	switch (current_context->curr_state)
	{
	case user_io_state_login:
		if (is_usrname_set() == false)
		{
			POWERMON_LOGGER(USER_IO, DEBUG, "Login credentials: username.\n", 0);
			set_usrname_valid();
			set_username(msg->data);
			user_io_fsm(user_io_input_login_name);
		}
		else if (is_passwd_set() == false)
		{
			POWERMON_LOGGER(USER_IO, DEBUG, "Login credentials: password.\n", 0);
			set_passwd_valid();
			set_password(msg->data);
			user_io_fsm(user_io_input_login_passw);
		}
		else
		{
			POWERMON_LOGGER(USER_IO, WARN, "Login state out of sync.\n", 0);
			current_context->context();
		}
		break;

	case user_io_state_report:
		configure_input(msg);
		user_io_fsm(user_io_input_key_numeric);
		break;

	default:
		POWERMON_LOGGER(USER_IO, DEBUG, "FSM error: Unexpected string data.\n", 0);
		user_io_fsm(user_io_input_unhandled);

		break;
	}
}

void user_io_fsm_register(
		user_io_state_e state,
		user_io_input_e input,
		user_io_action action)
{
	fsm_action[state][input] = action;
}

void user_io_ctxt_switch(io_context_t *new_context)
{
	POWERMON_LOGGER(USER_IO, TRACE, "user_io_ctxt_switch: New context: %s\n", new_context->ctxt_str);

	current_context = new_context;
}

void user_io_action_none(void)
{
	POWERMON_LOGGER(USER_IO, TRACE, "user_io_action_none\n", 0);

	/* Redraw the current context screen */
	current_context->context();
}

void user_io_fsm_init(void)
{
	POWERMON_LOGGER(USER_IO, TRACE, "user_io_fsm_init\n", 0);

	int state, input;
	for (state = user_io_state_first;
		state < user_io_state_count;
		state++)
	{
		for(input = user_io_input_first;
			input < user_io_input_count;
			input ++)
		{
			fsm_action[state][input] = user_io_action_none;
		}
	}
}

