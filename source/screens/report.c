/* ============================================================== */
/*
 * report.c
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
#include "powermon_time.h"


extern void consoleReportActiveNodes(void);
extern void consoleReportSystemStatus(void);

void report_screen(void)
{
	printf("\t ==========================\n");
	printf("\t|                          |\n");
	printf("\t|       Report Menu        |\n");
	printf("\t|                          |\n");
	printf("\t ==========================\n");
	printf("\t|   t - top menu           |\n");
	printf("\t|   r - refresh            |\n");
	printf("\t|   o - logout             |\n");
	printf("\t|   x - exit               |\n");
	printf("\t|                          |\n");
	printf("\t ==========================\n\n");
	printf("\t\t: ");
	fflush(stdout);

}

void report_active_nodes(void)
{
	time_t curr_tm = time(NULL);
	char *timeBuf = ctime(&curr_tm);

	PwrmonTime pwrmonTime;
	getPwrmonElapsedSystemTime(&pwrmonTime);

	printf("\n\n");
	printf("\t ========================================================================================= \n");
	printf("\t|                         |                  |  Active  |  Avg ON | Avg OFF | Avg |  H/B  |\n");
	printf("\t|    Serial Number        |      nodeIp      | Disable  |  mm:ss  |  mm:ss  | Amp | Alert |\n");
	printf("\t ========================================================================================= \n");
	/*                 1         2         3         4         5         6         7         8         9  */
	/*        1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901 */
	/*          1. 000000399098171-0001 |     10.0.0.8     |    No    |  14:34  |  30:23  | 23  |   *   | */

	consoleReportActiveNodes(); /* In powermon_calc.c */

	printf("\t ========================================================================================= \n");
	printf("\t System Statistics\n");
	printf("\t ========================================================================================= \n");
	consoleReportSystemStatus();
	printf("\t ========================================================================================= \n");
	printf("\t %s", timeBuf);
	printf("\t Elapsed time: %d days, %02d:%02d:%02d\n\n",
			pwrmonTime.days, pwrmonTime.hours, pwrmonTime.mins, pwrmonTime.secs);

	report_screen();
}
