/* ============================================================== */
/*
 * powermon.c
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
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "common.h"
#include "msg_queues.h"

pthread_t powermon_tid = (pthread_t)NULL;

/* =================================
 * void powermon_thread(void)
 */

void* powermon_thread(void *powermon_mq)
{
	printf("\tPowerMon: powermon_thread.\n");
	pthread_exit((void *)NULL);

	return (void *)NULL;
}

/* =================================
 * void powermon_thread_create(void)
 */
void powermon_thread_create(void)
{
	int err = pthread_create(
			&powermon_tid,
			NULL,
			&powermon_thread,
			NULL);

	if (err != 0)
		printf("\tUnable to create powermon_thread :[%s]\n", strerror(err));
}
