/* ============================================================== */
/*
 * powermon_time.h
 *
 * Copyright (c) 2018 David Hammond
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

#include <time.h>

#ifndef __POWERMON_TIME_H__
#define __POWERMON_TIME_H__

typedef struct PwrmonTime_t {
	double elapsedSecs;
	unsigned int days;
	unsigned int hours;
	unsigned int mins;
	unsigned int secs;
} PwrmonTime;


time_t getPwrmonSystemStartTime(void);
void getPwrmonElapsedSystemTime(PwrmonTime *elapsedTime);
double getPwrmonElapsedTime(time_t time1, time_t time0, PwrmonTime *elapsedTime);

#endif /* __POWERMON_TIME_H__ */
