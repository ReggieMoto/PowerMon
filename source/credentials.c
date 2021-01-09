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

#include <string.h>

#include "credentials.h"
#include "powermon_logger.h"
#include "user_io.h"

static credentials_t credentials;

bool is_usrname_set(void)
{
	return (credentials.login_flags.username == 1);
}

bool is_passwd_set(void)
{
	return (credentials.login_flags.password == 1);
}

void set_usrname_valid(void)
{
	credentials.login_flags.username = 1;
}

void set_passwd_valid(void)
{
	credentials.login_flags.password = 1;
}

void set_credentials_valid(void)
{
	POWERMON_LOGGER(DSTORE, TRACE, "Set credentials valid\n", 0);
	credentials.valid = pwr_mon_credentials_valid;
}

void set_credentials_invalid(void)
{
	POWERMON_LOGGER(DSTORE, TRACE, "Set credentials invalid\n", 0);
	credentials.valid = pwr_mon_credentials_invalid;
}

bool are_credentials_valid(void)
{
	POWERMON_LOGGER(DSTORE, TRACE, "credentials.valid: %d\n", credentials.valid);
	return (credentials.valid == pwr_mon_credentials_valid);
}

const char* get_username(void)
{
	return (credentials.username);
}

void set_username(const char *username)
{
	strncpy(credentials.username, username, MAX_KBD_INPUT_STR_LEN);
}

const char* get_password(void)
{
	return (credentials.password);
}

void set_password(const char *password)
{
	strncpy(credentials.password, password, MAX_KBD_INPUT_STR_LEN);
}

void reset_credentials(void)
{
	memset(&credentials, 0, sizeof(credentials_t));
}

void validate_credentials(void)
{
	user_io_send_credentials(&credentials);
}

