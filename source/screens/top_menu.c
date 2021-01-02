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

void top_menu_screen(void)
{
	printf("\n\n\t ======================\n");
	printf("\t|                      |\n");
	printf("\t|  PowerMon Top Menu   |\n");
	printf("\t|                      |\n");
	printf("\t ======================\n");
	printf("\t|                      |\n");
	printf("\t|     c - config       |\n");
	printf("\t|     r - report       |\n");
	printf("\t|   ----------------   |\n");
	printf("\t|     o - logout       |\n");
	printf("\t|     x - exit         |\n");
	printf("\t|                      |\n");
	printf("\t ======================\n\n");
	printf("\t\t: ");
	fflush(stdout);

}

void menu_prompt(void)
{
	printf("\t\t: ");
	fflush(stdout);
}
