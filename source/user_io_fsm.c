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

#include "user_io_fsm.h"
#include "pwr_mon_msg.h"
#include "credentials.h"
#include "powermon_logger.h"

extern void user_io_send_credentials(credentials_t *credentials);
extern void user_io_send_exit_msg(void);
extern void login_screen(void);
extern void menu_prompt(void);
extern void print_login_username(void);
extern void print_login_password(void);
extern void top_menu_screen(void);
extern void print_login_invalid(void);
extern void config_screen(void);
extern void user_screen(void);
extern void delete_user_screen(void);
extern void edit_user_screen(void);
extern void add_user_screen(void);
extern void report_screen(void);
extern void report_active_nodes(void);

static user_io_action_e user_io_action[user_io_state_count][user_io_input_count] = {
/* 0. user_io_state_init */
	{	user_io_action_login, /* user_io_input_start */
		user_io_action_none, /* user_io_input_login_name */
		user_io_action_none, /* user_io_input_login_passw */
		user_io_action_none, /* user_io_input_login_valid */
		user_io_action_none, /* user_io_input_login_restart/invalid */
		user_io_action_none, /* user_io_input_key_b */
		user_io_action_none, /* user_io_input_key_c */
		user_io_action_none, /* user_io_input_key_d */
		user_io_action_none, /* user_io_input_key_l */
		user_io_action_none, /* user_io_input_key_m */
		user_io_action_none, /* user_io_input_key_n */
		user_io_action_none, /* user_io_input_key_o */
		user_io_action_none, /* user_io_input_key_r */
		user_io_action_none, /* user_io_input_key_t */
		user_io_action_none, /* user_io_input_key_u */
		user_io_action_none  /* user_io_input_key_x */
	},
	/* 1. user_io_state_first_login */
	{	user_io_action_none, /* user_io_input_start */
		user_io_action_none, /* user_io_input_login_name */
		user_io_action_none, /* user_io_input_login_passw */
		user_io_action_none, /* user_io_input_login_valid */
		user_io_action_none, /* user_io_input_login_restart/invalid */
		user_io_action_none, /* user_io_input_key_b */
		user_io_action_none, /* user_io_input_key_c */
		user_io_action_none, /* user_io_input_key_d */
		user_io_action_none, /* user_io_input_key_l */
		user_io_action_none, /* user_io_input_key_m */
		user_io_action_none, /* user_io_input_key_n */
		user_io_action_none, /* user_io_input_key_o */
		user_io_action_none, /* user_io_input_key_r */
		user_io_action_none, /* user_io_input_key_t */
		user_io_action_none, /* user_io_input_key_u */
		user_io_action_none  /* user_io_input_key_x */
	},
/* 2. user_io_state_login */
	{	user_io_action_none, /* user_io_input_start */
		user_io_action_username, /* user_io_input_login_name */
		user_io_action_password, /* user_io_input_login_passw */
		user_io_action_goto_top, /* user_io_input_login_valid */
		user_io_action_login, /* user_io_input_login_restart/invalid */
		user_io_action_none, /* user_io_input_key_b */
		user_io_action_none, /* user_io_input_key_c */
		user_io_action_none, /* user_io_input_key_d */
		user_io_action_none, /* user_io_input_key_l */
		user_io_action_none, /* user_io_input_key_m */
		user_io_action_none, /* user_io_input_key_n */
		user_io_action_none, /* user_io_input_key_o */
		user_io_action_none, /* user_io_input_key_r */
		user_io_action_none, /* user_io_input_key_t */
		user_io_action_none, /* user_io_input_key_u */
		user_io_action_none  /* user_io_input_key_x */
	},
/* 3. user_io_state_main */
	{	user_io_action_none, /* user_io_input_start */
		user_io_action_none, /* user_io_input_login_name */
		user_io_action_none, /* user_io_input_login_passw */
		user_io_action_none, /* user_io_input_login_valid */
		user_io_action_none, /* user_io_input_login_restart/invalid */
		user_io_action_none, /* user_io_input_key_b */
		user_io_action_config,   /* user_io_input_key_c */
		user_io_action_none, /* user_io_input_key_d */
		user_io_action_none, /* user_io_input_key_l */
		user_io_action_none, /* user_io_input_key_m */
		user_io_action_none, /* user_io_input_key_n */
		user_io_action_logout, /* user_io_input_key_o */
		user_io_action_report, /* user_io_input_key_r */
		user_io_action_none, /* user_io_input_key_t */
		user_io_action_none, /* user_io_input_key_u */
		user_io_action_exit  /* user_io_input_key_x */
	},
/* 4. user_io_state_config */
	{	user_io_action_none, /* user_io_input_start */
		user_io_action_none, /* user_io_input_login_name */
		user_io_action_none, /* user_io_input_login_passw */
		user_io_action_none, /* user_io_input_login_valid */
		user_io_action_none, /* user_io_input_login_restart/invalid */
		user_io_action_none, /* user_io_input_key_b */
		user_io_action_none, /* user_io_input_key_c */
		user_io_action_none, /* user_io_input_key_d */
		user_io_action_none, /* user_io_input_key_l */
		user_io_action_none, /* user_io_input_key_m */
		user_io_action_none, /* user_io_input_key_n */
		user_io_action_logout, /* user_io_input_key_o */
		user_io_action_none, /* user_io_input_key_r */
		user_io_action_goto_top, /* user_io_input_key_t */
		user_io_action_cfg_user, /* user_io_input_key_u */
		user_io_action_exit  /* user_io_input_key_x */
	},
/* 5. user_io_state_report */
	{	user_io_action_none, /* user_io_input_start */
		user_io_action_none, /* user_io_input_login_name */
		user_io_action_none, /* user_io_input_login_passw */
		user_io_action_none, /* user_io_input_login_valid */
		user_io_action_none, /* user_io_input_login_restart/invalid */
		user_io_action_none, /* user_io_input_key_b */
		user_io_action_none, /* user_io_input_key_c */
		user_io_action_none, /* user_io_input_key_d */
		user_io_action_none, /* user_io_input_key_l */
		user_io_action_none, /* user_io_input_key_m */
		user_io_action_none, /* user_io_input_key_n */
		user_io_action_logout, /* user_io_input_key_o */
		user_io_action_report, /* user_io_input_key_r */
		user_io_action_goto_top, /* user_io_input_key_t */
		user_io_action_none, /* user_io_input_key_u */
		user_io_action_exit  /* user_io_input_key_x */
	},
/* 6. user_io_state_cfg_user */
	{	user_io_action_none, /* user_io_input_start */
		user_io_action_none, /* user_io_input_login_name */
		user_io_action_none, /* user_io_input_login_passw */
		user_io_action_none, /* user_io_input_login_valid */
		user_io_action_none, /* user_io_input_login_restart/invalid */
		user_io_action_none, /* user_io_input_key_b */
		user_io_action_none, /* user_io_input_key_c */
		user_io_action_user_delete, /* user_io_input_key_d */
		user_io_action_none, /* user_io_input_key_l */
		user_io_action_none, /* user_io_input_key_m */
		user_io_action_user_add, /* user_io_input_key_n */
		user_io_action_logout, /* user_io_input_key_o */
		user_io_action_none, /* user_io_input_key_r */
		user_io_action_goto_top, /* user_io_input_key_t */
		user_io_action_none, /* user_io_input_key_u */
		user_io_action_exit  /* user_io_input_key_x */
	},
/* 7. user_io_state_cfg_logging */
	{	user_io_action_none, /* user_io_input_start */
		user_io_action_none, /* user_io_input_login_name */
		user_io_action_none, /* user_io_input_login_passw */
		user_io_action_none, /* user_io_input_login_valid */
		user_io_action_none, /* user_io_input_login_restart/invalid */
		user_io_action_none, /* user_io_input_key_b */
		user_io_action_none, /* user_io_input_key_c */
		user_io_action_none, /* user_io_input_key_d */
		user_io_action_none, /* user_io_input_key_l */
		user_io_action_none, /* user_io_input_key_m */
		user_io_action_none, /* user_io_input_key_n */
		user_io_action_none, /* user_io_input_key_o */
		user_io_action_none, /* user_io_input_key_r */
		user_io_action_none, /* user_io_input_key_t */
		user_io_action_none, /* user_io_input_key_u */
		user_io_action_none  /* user_io_input_key_x */
	},
/* 8. user_io_state_new_user */
	{	user_io_action_none, /* user_io_input_start */
		user_io_action_none, /* user_io_input_login_name */
		user_io_action_none, /* user_io_input_login_passw */
		user_io_action_none, /* user_io_input_login_valid */
		user_io_action_none, /* user_io_input_login_restart/invalid */
		user_io_action_cfg_user, /* user_io_input_key_b */
		user_io_action_none, /* user_io_input_key_c */
		user_io_action_none, /* user_io_input_key_d */
		user_io_action_none, /* user_io_input_key_l */
		user_io_action_none, /* user_io_input_key_m */
		user_io_action_none, /* user_io_input_key_n */
		user_io_action_logout, /* user_io_input_key_o */
		user_io_action_none, /* user_io_input_key_r */
		user_io_action_goto_top, /* user_io_input_key_t */
		user_io_action_none, /* user_io_input_key_u */
		user_io_action_exit  /* user_io_input_key_x */
	},
/* 9. user_io_state_modify_user */
	{	user_io_action_none, /* user_io_input_start */
		user_io_action_none, /* user_io_input_login_name */
		user_io_action_none, /* user_io_input_login_passw */
		user_io_action_none, /* user_io_input_login_valid */
		user_io_action_none, /* user_io_input_login_restart/invalid */
		user_io_action_cfg_user, /* user_io_input_key_b */
		user_io_action_none, /* user_io_input_key_c */
		user_io_action_none, /* user_io_input_key_d */
		user_io_action_none, /* user_io_input_key_l */
		user_io_action_none, /* user_io_input_key_m */
		user_io_action_none, /* user_io_input_key_n */
		user_io_action_logout, /* user_io_input_key_o */
		user_io_action_none, /* user_io_input_key_r */
		user_io_action_goto_top, /* user_io_input_key_t */
		user_io_action_none, /* user_io_input_key_u */
		user_io_action_exit  /* user_io_input_key_x */
	},
/* 10. user_io_state_delete_user */
	{	user_io_action_none, /* user_io_input_start */
		user_io_action_none, /* user_io_input_login_name */
		user_io_action_none, /* user_io_input_login_passw */
		user_io_action_none, /* user_io_input_login_valid */
		user_io_action_none, /* user_io_input_login_restart/invalid */
		user_io_action_cfg_user, /* user_io_input_key_b */
		user_io_action_none, /* user_io_input_key_c */
		user_io_action_none, /* user_io_input_key_d */
		user_io_action_none, /* user_io_input_key_l */
		user_io_action_none, /* user_io_input_key_m */
		user_io_action_none, /* user_io_input_key_n */
		user_io_action_logout, /* user_io_input_key_o */
		user_io_action_none, /* user_io_input_key_r */
		user_io_action_goto_top, /* user_io_input_key_t */
		user_io_action_none, /* user_io_input_key_u */
		user_io_action_exit  /* user_io_input_key_x */
	},
/* 11. user_io_state_log_level */
	{	user_io_action_none, /* user_io_input_start */
		user_io_action_none, /* user_io_input_login_name */
		user_io_action_none, /* user_io_input_login_passw */
		user_io_action_none, /* user_io_input_login_valid */
		user_io_action_none, /* user_io_input_login_restart/invalid */
		user_io_action_none, /* user_io_input_key_b */
		user_io_action_none, /* user_io_input_key_c */
		user_io_action_none, /* user_io_input_key_d */
		user_io_action_none, /* user_io_input_key_l */
		user_io_action_none, /* user_io_input_key_m */
		user_io_action_none, /* user_io_input_key_n */
		user_io_action_none, /* user_io_input_key_o */
		user_io_action_none, /* user_io_input_key_r */
		user_io_action_none, /* user_io_input_key_t */
		user_io_action_none, /* user_io_input_key_u */
		user_io_action_none  /* user_io_input_key_x */
	},
/* 12. user_io_state_log_comp */
	{	user_io_action_none, /* user_io_input_start */
		user_io_action_none, /* user_io_input_login_name */
		user_io_action_none, /* user_io_input_login_passw */
		user_io_action_none, /* user_io_input_login_valid */
		user_io_action_none, /* user_io_input_login_restart/invalid */
		user_io_action_none, /* user_io_input_key_b */
		user_io_action_none, /* user_io_input_key_c */
		user_io_action_none, /* user_io_input_key_d */
		user_io_action_none, /* user_io_input_key_l */
		user_io_action_none, /* user_io_input_key_m */
		user_io_action_none, /* user_io_input_key_n */
		user_io_action_none, /* user_io_input_key_o */
		user_io_action_none, /* user_io_input_key_r */
		user_io_action_none, /* user_io_input_key_t */
		user_io_action_none, /* user_io_input_key_u */
		user_io_action_none  /* user_io_input_key_x */
	},
/* 13. user_io_state_log_dirs */
	{	user_io_action_none, /* user_io_input_start */
		user_io_action_none, /* user_io_input_login_name */
		user_io_action_none, /* user_io_input_login_passw */
		user_io_action_none, /* user_io_input_login_valid */
		user_io_action_none, /* user_io_input_login_restart/invalid */
		user_io_action_none, /* user_io_input_key_b */
		user_io_action_none, /* user_io_input_key_c */
		user_io_action_none, /* user_io_input_key_d */
		user_io_action_none, /* user_io_input_key_l */
		user_io_action_none, /* user_io_input_key_m */
		user_io_action_none, /* user_io_input_key_n */
		user_io_action_none, /* user_io_input_key_o */
		user_io_action_none, /* user_io_input_key_r */
		user_io_action_none, /* user_io_input_key_t */
		user_io_action_none, /* user_io_input_key_u */
		user_io_action_none  /* user_io_input_key_x */
	}
};

static char *user_io_fsm_state_str[] = {
	"user_io_state_init",
	"user_io_state_first_login",
	"user_io_state_login",
	"user_io_state_main",
	"user_io_state_config",
	"user_io_state_report",
	"user_io_state_cfg_user",
	"user_io_state_cfg_logging",
	"user_io_state_new_user",
	"user_io_state_modify_user",
	"user_io_state_delete_user",
	"user_io_state_log_level",
	"user_io_state_log_comp",
	"user_io_state_log_dirs"
};

static char * get_user_io_fsm_state_str(user_io_state_e state)
{
	return user_io_fsm_state_str[state];
}

static char *user_io_fsm_input_str[] = {
	"user_io_input_restart",
	"user_io_input_login_name",
	"user_io_input_login_passw",
	"user_io_input_login_valid",
	"user_io_input_login_invalid",
	"user_io_input_key_c",
	"user_io_input_key_r",
	"user_io_input_key_l",
	"user_io_input_key_x",
	"user_io_input_key_u",
	"user_io_input_key_m",
	"user_io_input_key_d",
	"user_io_input_key_n",
	"user_io_input_key_b",
	"user_io_input_key_t"
};

static char * get_user_io_fsm_input_str(user_io_input_e input)
{
	return user_io_fsm_input_str[input];
}

static pwrmon_msg_t fsm_input_msg;
static credentials_t credentials;

static struct _login_flags {
	unsigned int username: 1;
	unsigned int password: 1;
} login_flags;

static user_io_state_e state = user_io_state_init;

void user_io_fsm(user_io_input_e input)
{
	user_io_action_e action;
	action = user_io_action[state][input];

	POWERMON_LOGGER(USER_IO, DEBUG, "State:  %s\n", get_user_io_fsm_state_str(state));
	POWERMON_LOGGER(USER_IO, DEBUG, "Input:  %s\n", get_user_io_fsm_input_str(input));

	switch(action) {

	case user_io_action_login:
	case user_io_action_logout:
		POWERMON_LOGGER(USER_IO, DEBUG, "Action: user_io_action_login\n", 0);
		memset(&credentials, 0, sizeof(credentials_t));
		login_flags.username = 0;
		login_flags.password = 0;
		login_screen();
		print_login_username();
		state = user_io_state_login;
		break;

	case user_io_action_username:
		POWERMON_LOGGER(USER_IO, DEBUG, "Action: user_io_action_username\n", 0);
		print_login_password();
		break;

	case user_io_action_password:
		POWERMON_LOGGER(USER_IO, DEBUG, "Action: user_io_action_password\n", 0);
		credentials.valid = pwr_mon_credentials_invalid;
		user_io_send_credentials(&credentials);
		break;

	case user_io_action_config:
		POWERMON_LOGGER(USER_IO, DEBUG, "Action: user_io_action_config\n", 0);
		config_screen();
		state = user_io_state_config;
		break;

	case user_io_action_report:
		POWERMON_LOGGER(USER_IO, DEBUG, "Action: user_io_action_report\n", 0);
		if (state != user_io_state_report)
		{
			report_screen();
			report_active_nodes();
			state = user_io_state_report;
		}
		else
			report_active_nodes();
		break;
#if 0
	case user_io_action_logout:
		POWERMON_LOGGER(USER_IO, DEBUG, "Action: user_io_action_logout\n", 0);
		break;
#endif
	case user_io_action_exit:
		POWERMON_LOGGER(USER_IO, DEBUG, "Action: user_io_action_exit\n", 0);
		user_io_send_exit_msg();
		break;

	case user_io_action_cfg_user:
		POWERMON_LOGGER(USER_IO, DEBUG, "Action: user_io_action_cfg_user\n", 0);
		user_screen();
		state = user_io_state_cfg_user;

		break;

	case user_io_action_cfg_logging:
		POWERMON_LOGGER(USER_IO, DEBUG, "Action: user_io_action_cfg_logging\n", 0);
		break;

	case user_io_action_user_passwd:
		POWERMON_LOGGER(USER_IO, DEBUG, "Action: user_io_action_user_passwd\n", 0);
		break;

	case user_io_action_user_add:
		POWERMON_LOGGER(USER_IO, DEBUG, "Action: user_io_action_user_add\n", 0);
		add_user_screen();
		break;

	case user_io_action_user_mod:
		POWERMON_LOGGER(USER_IO, DEBUG, "Action: user_io_action_user_mod\n", 0);
		edit_user_screen();
		break;

	case user_io_action_user_delete:
		POWERMON_LOGGER(USER_IO, DEBUG, "Action: user_io_action_user_delete\n", 0);
		delete_user_screen();
		break;

	case user_io_action_log_level:
		POWERMON_LOGGER(USER_IO, DEBUG, "Action: user_io_action_log_level\n", 0);
		break;

	case user_io_action_log_comp:
		POWERMON_LOGGER(USER_IO, DEBUG, "Action: user_io_action_log_comp\n", 0);
		break;

	case user_io_action_log_dirs:
		POWERMON_LOGGER(USER_IO, DEBUG, "Action: user_io_action_log_dirs\n", 0);
		break;

	case user_io_action_goto_top:
		POWERMON_LOGGER(USER_IO, DEBUG, "Action: user_io_action_goto_top\n", 0);
		top_menu_screen();
		state = user_io_state_main;
		break;

	case user_io_action_reset:
		POWERMON_LOGGER(USER_IO, DEBUG, "Action: user_io_action_reset\n", 0);
		break;

	case user_io_action_none:
		POWERMON_LOGGER(USER_IO, DEBUG, "Action: user_io_action_none\n", 0);
		menu_prompt();
		break;

	default:
		POWERMON_LOGGER(USER_IO, DEBUG, "Action: default (invalid action)\n", 0);
		break;
	}
}

void user_io_fsm_process_string(pwrmon_msg_t *msg)
{
	memcpy(&fsm_input_msg, msg, sizeof(pwrmon_msg_t));

	switch (state)
	{
	case user_io_state_login:
		if (login_flags.username == 0)
		{
			POWERMON_LOGGER(USER_IO, DEBUG, "Login credentials: username.\n", 0);
			login_flags.username = 1;
			strcpy(credentials.username, msg->data);
			user_io_fsm(user_io_input_login_name);
		}
		else if (login_flags.password == 0)
		{
			POWERMON_LOGGER(USER_IO, DEBUG, "Login credentials: password.\n", 0);
			login_flags.password = 1;
			strcpy(credentials.password, msg->data);
			user_io_fsm(user_io_input_login_passw);
		}
		else
		{
			credentials_t *credentials = (credentials_t *)msg->data;
			if (credentials->valid == pwr_mon_credentials_valid)
			{
				POWERMON_LOGGER(USER_IO, DEBUG, "Login credentials are valid.\n", 0);
				user_io_fsm(user_io_input_login_valid);
			}
			else
			{
				POWERMON_LOGGER(USER_IO, DEBUG, "Login credentials are not valid.\n", 0);
				print_login_invalid();
				user_io_fsm(user_io_input_login_invalid);
			}
		}
		break;
	default:
		POWERMON_LOGGER(USER_IO, WARN, "FSM error: Unexpected string data.\n", 0);
		break;
	}
}
