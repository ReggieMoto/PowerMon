/* ============================================================== */
/*
 * config.c
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

void config_screen(void)
{
	printf("\n\n");
	printf("\t ==========================\n");
	printf("\t|                          |\n");
	printf("\t|       Config Menu        |\n");
	printf("\t|                          |\n");
	printf("\t ==========================\n");
	printf("\t|                          |\n");
	printf("\t|   u - user               |\n");
	printf("\t|   l - logging            |\n");
	printf("\t|   d - device discovery   |\n");
	printf("\t|   c - connectivity       |\n");
	printf("\t|   r - reset              |\n");
	printf("\t|   --------------------   |\n");
	printf("\t|   t - top menu           |\n");
	printf("\t|   o - logout             |\n");
	printf("\t|   x - exit               |\n");
	printf("\t|                          |\n");
	printf("\t ==========================\n\n");
	printf("\t\t: ");
	fflush(stdout);

}
