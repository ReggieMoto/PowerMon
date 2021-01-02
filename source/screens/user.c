/* ============================================================== */
/*
 * user.c
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

void user_screen(void)
{
	printf("\n\n");
	printf("\t ==========================\n");
	printf("\t|                          |\n");
	printf("\t|        User Menu         |\n");
	printf("\t|                          |\n");
	printf("\t ==========================\n");
	printf("\t|                          |\n");
	printf("\t|      n - new user        |\n");
	printf("\t|      e - edit user       |\n");
	printf("\t|      d - delete user     |\n");
	printf("\t|      p - password        |\n");
	printf("\t|   --------------------   |\n");
	printf("\t|      t - top menu        |\n");
	printf("\t|      o - logout          |\n");
	printf("\t|      x - exit            |\n");
	printf("\t|                          |\n");
	printf("\t ==========================\n\n");
	printf("\t\t: ");
	fflush(stdout);
}

void add_user_screen(void)
{
	printf("\n\n");
	printf("\t ==========================\n");
	printf("\t|                          |\n");
	printf("\t|      Add User Menu       |\n");
	printf("\t|                          |\n");
	printf("\t ==========================\n");
	printf("\t|                          |\n");
	printf("\t|     n - name             |\n");
	printf("\t|     e - email address    |\n");
	printf("\t|     d - phone            |\n");
	printf("\t|     u - username         |\n");
	printf("\t|     p - password         |\n");
	printf("\t|   --------------------   |\n");
	printf("\t|     b - back             |\n");
	printf("\t|     t - top menu         |\n");
	printf("\t|     o - logout           |\n");
	printf("\t|     x - exit             |\n");
	printf("\t|                          |\n");
	printf("\t ==========================\n\n");
	printf("\t\t: ");
	fflush(stdout);
}

void edit_user_screen(void)
{
	printf("\n\n");
	printf("\t ==========================\n");
	printf("\t|                          |\n");
	printf("\t|      Edit User Menu      |\n");
	printf("\t|                          |\n");
	printf("\t ==========================\n");
	printf("\t|                          |\n");
	printf("\t|     n - name             |\n");
	printf("\t|     e - email address    |\n");
	printf("\t|     d - phone            |\n");
	printf("\t|     p - password         |\n");
	printf("\t|   --------------------   |\n");
	printf("\t|     b - back             |\n");
	printf("\t|     t - top menu         |\n");
	printf("\t|     o - logout           |\n");
	printf("\t|     x - exit             |\n");
	printf("\t|                          |\n");
	printf("\t ==========================\n\n");
	printf("\t\t: ");
	fflush(stdout);
}

void delete_user_screen(void)
{
	printf("\n\n");
	printf("\t ==========================\n");
	printf("\t|                          |\n");
	printf("\t|     Delete User Menu     |\n");
	printf("\t|                          |\n");
	printf("\t ==========================\n");
	printf("\t|                          |\n");
	printf("\t|       n - name           |\n");
	printf("\t|   --------------------   |\n");
	printf("\t|       b - back           |\n");
	printf("\t|       t - top menu       |\n");
	printf("\t|       o - logout         |\n");
	printf("\t|       x - exit           |\n");
	printf("\t|                          |\n");
	printf("\t ==========================\n\n");
	printf("\t\t: ");
	fflush(stdout);
}
