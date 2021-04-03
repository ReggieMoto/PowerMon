/* ============================================================== */
/*
 * user_io_fsm.h
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

#ifndef __USER_IO_FSM_H__
#define __USER_IO_FSM_H__

#include "pwr_mon_msg.h"

typedef enum {
	user_io_state_first,
	user_io_state_init = user_io_state_first,
	user_io_state_login,
	user_io_state_main,
		user_io_state_user,
			user_io_state_new_user,
			user_io_state_modify_user,
			user_io_state_delete_user,
		user_io_state_logging,
			user_io_state_log_level,
			user_io_state_log_comp,
		user_io_state_discovery,
		user_io_state_connectivity,
		user_io_state_report,
	user_io_state_last = user_io_state_report,
	user_io_state_count
} user_io_state_e;

typedef enum {
	user_io_input_first,
	user_io_input_login_start = user_io_input_first,
	user_io_input_login_name,
	user_io_input_login_passw,
	user_io_input_login_valid,
	user_io_input_login_invalid,
	user_io_input_key_b,
	user_io_input_key_c,
	user_io_input_key_d,
	user_io_input_key_e,
	user_io_input_key_l,
	user_io_input_key_m,
	user_io_input_key_n,
	user_io_input_key_o,
	user_io_input_key_p,
	user_io_input_key_r,
	user_io_input_key_t,
	user_io_input_key_u,
	user_io_input_key_x,
	user_io_input_key_1,
	user_io_input_key_2,
	user_io_input_key_3,
	user_io_input_key_4,
	user_io_input_key_5,
	user_io_input_key_6,
	user_io_input_key_7,
	user_io_input_key_8,
	user_io_input_key_9,
	user_io_input_key_numeric,
	user_io_input_unhandled,
	user_io_input_last = user_io_input_unhandled,
	user_io_input_count
} user_io_input_e;

#if 0
typedef enum {
	user_io_action_first,
	user_io_action_login = user_io_action_first,
	user_io_action_username,
	user_io_action_password,
	user_io_action_config,
	user_io_action_report,
	user_io_action_logout,
	user_io_action_exit,
	user_io_action_cfg_user,
	user_io_action_cfg_logging,
	user_io_action_user_passwd,
	user_io_action_user_add,
	user_io_action_user_mod,
	user_io_action_user_delete,
	user_io_action_log_level,
	user_io_action_log_comp,
	user_io_action_log_dirs,
	user_io_action_goto_top,
	user_io_action_reset,
	user_io_action_last = user_io_action_none,
	user_io_action_count
} user_io_action_e;
#endif

typedef void (* io_fsm_ctxt)(void);

#define MAX_CTXT_STR_LEN 16u

typedef struct io_context_s {
	io_fsm_ctxt context;
	char ctxt_str[MAX_CTXT_STR_LEN];
	user_io_state_e curr_state;
	pwrmon_msg_t msg;
} io_context_t;

extern io_context_t login_ctxt;
extern io_context_t config_ctxt;
extern io_context_t report_ctxt;

typedef void (* user_io_action)(void);
void user_io_action_none(void);

void user_io_fsm(user_io_input_e input);
void user_io_fsm_register(
		user_io_state_e state,
		user_io_input_e input,
		user_io_action action);

void user_io_ctxt_switch(io_context_t *new_context);

void user_io_fsm_process_string(pwrmon_msg_t *msg);
void user_io_fsm(user_io_input_e input);
void user_io_fsm_init(void);

#endif /* __USER_IO_FSM_H__ */
