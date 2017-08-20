/* ============================================================== */
/*
 * wifi_io.c
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

pthread_t wifi_io_tid = (pthread_t)NULL;

/* =================================
 * void wifi_io_thread(void)
 */

void* wifi_io_thread(void *arg)
{
	printf("\tPowerMon: wifi_io_thread.\n");
	pthread_exit((void *)NULL);

	return (void *)NULL;
}

/* =================================
 * void wifi_io_thread_create(void)
 */

void wifi_io_thread_create(void)
{
    int err = pthread_create(
    		&wifi_io_tid,
    		NULL,
			&wifi_io_thread,
			NULL);

    if (err != 0)
        printf("\tUnable to create wifi_io_thread :[%s]\n", strerror(err));
}
