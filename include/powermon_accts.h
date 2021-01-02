/* ============================================================== */
/*
 * powermon_accts.h
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

#ifndef __POWERMON_ACCTS_H__
#define __POWERMON_ACCTS_H__

#define USER_NAME_LEN 25
#define SHA3_HASH_LEN 200
#define MAX_LOGIN_ACCOUNTS 10

typedef enum acount_type {
	root,
	user,
	none
} AccountType;

typedef struct login_account {

	char user[USER_NAME_LEN];
	AccountType type;
	char hash[SHA3_HASH_LEN];

} LoginAccount;

typedef struct login_accounts {

	LoginAccount account[MAX_LOGIN_ACCOUNTS];

} LoginAccounts;

#endif /* __POWERMON_ACCTS_H__ */
