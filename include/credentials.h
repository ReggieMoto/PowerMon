/* ============================================================== */
/*
 * credentials.h
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

#ifndef __CREDENTIALS_H__
#define __CREDENTIALS_H__

#include <stdbool.h>
#include "common.h"

typedef enum {
	pwr_mon_credentials_first,
	pwr_mon_credentials_invalid = pwr_mon_credentials_first,
	pwr_mon_credentials_valid,
	pwr_mon_credentials_query,
	pwr_mon_credentials_last = pwr_mon_credentials_query
} pwr_mon_credentials_e;

typedef struct _login_flags {
	unsigned int username: 1;
	unsigned int password: 1;
} login_flags_t;

typedef struct credentials_s {
	login_flags_t login_flags;

	char username[MAX_KBD_INPUT_STR_LEN];
	char password[MAX_KBD_INPUT_STR_LEN];
	pwr_mon_credentials_e valid;
} credentials_t;

bool is_usrname_set(void);
bool is_passwd_set(void);
void set_usrname_valid(void);
void set_passwd_valid(void);
void set_credentials_valid(void);
void set_credentials_invalid(void);
bool are_credentials_valid(void);
const char* get_username(void);
void set_username(const char *username);
const char* get_password(void);
void set_password(const char *password);
void validate_credentials(void);
void reset_credentials(void);

#endif /* __CREDENTIALS_H__ */
