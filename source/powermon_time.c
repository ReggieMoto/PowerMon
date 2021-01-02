/* ============================================================== */
/*
 * powermon_time.c
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
#include <memory.h>
#include "powermon_time.h"

time_t getPwrmonSystemStartTime(void)
{
	static time_t startTime = 0;
	if (startTime == 0)
	{
		tzset();
		time_t timeErr = time(&startTime);

		if (timeErr < 0)
		{
			startTime = 0;
		}
	}
	return startTime;
}

/* ================================================ */
/*
 *  getPwrmonElapsedTime
 *
 *  IN:
 *  time_t time1 -- latest time
 *  time_t time0 -- earliest time
 *
 *  OUT:
 *  double timeDiff -- difference in seconds between time1/curr_tm and time0
 *  elapsedTime -- If present return timeDiff in days/hrs/mins/secs
 */
/* ================================================ */
double getPwrmonElapsedTime(time_t time1, time_t time0, PwrmonTime *elapsedTime)
{
	unsigned int elapsed_hour_secs;
	unsigned int elapsed_mins_secs;

	/* difftime() returns the number of seconds elapsed between curr_tm and time0 */
	double timeDiff = difftime(time1, time0);

	if (elapsedTime)
	{
		elapsedTime->elapsedSecs = timeDiff;

		elapsedTime->days = elapsedTime->elapsedSecs/(24*60*60);
		elapsed_hour_secs = elapsedTime->elapsedSecs - (elapsedTime->days * (24*60*60));
		elapsedTime->hours = elapsed_hour_secs/(60*60);
		elapsed_mins_secs = elapsed_hour_secs - (elapsedTime->hours * (60*60));
		elapsedTime->mins = elapsed_mins_secs/60;
		elapsedTime->secs = elapsed_mins_secs%60;
	}

	return timeDiff;
}

void getPwrmonElapsedSystemTime(PwrmonTime *elapsedTime)
{
	time_t curr_tm;
	time_t timeErr = time(&curr_tm);
	if (timeErr < 0)
	{
		memset(elapsedTime, 0, sizeof(PwrmonTime));
	}
	else {
		time_t start_tm = getPwrmonSystemStartTime();
		getPwrmonElapsedTime(curr_tm, start_tm, elapsedTime);
	}
}
