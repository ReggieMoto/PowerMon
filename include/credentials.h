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

#include "common.h"

typedef enum {
	pwr_mon_credentials_first,
	pwr_mon_credentials_invalid = pwr_mon_credentials_first,
	pwr_mon_credentials_valid,
	pwr_mon_credentials_query,
	pwr_mon_credentials_last = pwr_mon_credentials_query
} pwr_mon_credentials_e;

typedef struct
{
	char username[MAX_KBD_INPUT_STR_LEN];
	char password[MAX_KBD_INPUT_STR_LEN];
	pwr_mon_credentials_e valid;
} credentials_t;

#endif /* __CREDENTIALS_H__ */
