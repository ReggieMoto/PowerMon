/* ============================================================== */
/*
 * user_io.c
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

const char* user_io_mq_name = "user_io";
char user_io_mq_buffer[MAX_MESSAGE_SIZE];
mqd_t user_io_mq = (mqd_t)0;

pthread_t user_io_tid = (pthread_t)NULL;

/* =================================
 * void user_io_thread(void)
 */

void* user_io_thread(void *user_io_mq)
{
	int msgq_is_active = TRUE;
	size_t msg_size;
	mqd_t msg_q = *(mqd_t *)user_io_mq;

	printf("\tPowerMon: user_io_thread.\n");

	/* This is the main power mon thread loop
	 *
	 */
	do {
		/* Wait on keyboard input */
		printf("\tuser_io_thread: Waiting to receive a keypress.\n");
		char keypress = fgetc(stdin);
		printf("\tuser_io_thread: keypress received.\n");

		/* Notify the main power mon thread */
		//int send_err = mq_send(pwr_mon_mq, &keypress, 1, 0);

		/* Wait to receive a message */
		/*
		printf("\tuser_io_thread: Waiting to receive a message.\n");

		msg_size = mq_receive(
				pwr_mon_mq,
				pwr_mon_mq_buffer,
				(size_t) MAX_MESSAGE_SIZE,
				(unsigned int *)NULL);
		printf("\tuser_io_thread: Message received.\n");

		*/

		msgq_is_active = FALSE;
	} while (msgq_is_active);

	pthread_exit((void *)NULL);

	return (void *)NULL;
}

/* =================================
 * void user_io_thread_create(void)
 */
void user_io_thread_create(void)
{
	/* The main power monitor message queue */
	/* The user I/O, WIFI I/O, and data store threads can write to it. */
	mqd_t user_io_mq = mq_open(user_io_mq_name, O_RDONLY);

	if (user_io_mq)
	{
		int err = pthread_create(
				&user_io_tid,
				NULL,
				&user_io_thread,
				(void *)&user_io_mq);

		if (err != 0)
			printf("\tUnable to create user_io_thread :[%s]\n", strerror(err));

		mq_close(user_io_mq);
		mq_unlink(user_io_mq_name);
	}
	else
		printf("Unable to open user I/O message queue.\n");
}
