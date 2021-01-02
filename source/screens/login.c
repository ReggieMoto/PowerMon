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

#include <stdio.h>

void print_login_username(void)
{
	printf("\tusername: ");
	fflush(stdout);
}

void print_login_password(void)
{
	printf("\tpassword: ");
	fflush(stdout);
}

void print_login_invalid(void)
{
	printf("\tLogin credentials invalid.\n");
	fflush(stdout);
}

void login_screen(void)
{
	printf("\n\n");
	printf("\t ======================\n");
	printf("\t|    PowerMon Login    |\n");
	printf("\t ======================\n");
	fflush(stdout);
}
