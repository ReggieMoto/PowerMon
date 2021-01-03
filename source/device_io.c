/* ============================================================== */
/*
 * device_io.c
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
#include <semaphore.h>
#include <time.h>

#include "common.h"
#include "msg_queues.h"
#include "pwr_mon_msg.h"
#include "powermon_logger.h"

extern pthread_t device_io_data_thread_create(unsigned int *data_thread_active);

static pthread_t device_io_tid = (pthread_t)NULL;

/* =================================
 * process_received_msg
 */

static unsigned int process_received_msg(const pwrmon_msg_t *msg, const char msgLen)
{
	unsigned int thread_active = TRUE;

	switch(msg->id) {

	case pwr_mon_msg_id_exit:
		thread_active = FALSE;
		break;

	default:
		thread_active = TRUE;
		break;
	}

	return thread_active;
}

/* =================================
 * void device_io_thread(void)
 */

void* device_io_thread(void *arg)
{
	sem_t *sem = (sem_t *)arg;
	msg_q_status_e status;
	pthread_t device_io_data_tid = (pthread_t)NULL;
	unsigned int thread_active = TRUE;
	unsigned int data_thread_active = TRUE;
	/* char *msg_src; */

	/* Register the device_io message queue */
	status = msg_q_open(msg_q_client_device_io);

	if (status == msg_q_status_success)
	{
		sem_wait(sem);
		POWERMON_LOGGER(DEV_IO, THREAD, "device_io_thread is alive.\n",0);

		device_io_data_tid = device_io_data_thread_create(&data_thread_active);

		do {
			static char msg[MAX_MESSAGE_SIZE];
			unsigned int msgLen = MAX_MESSAGE_SIZE;

			memset(msg, 0, msgLen);

			POWERMON_LOGGER(DEV_IO, DEBUG, "Waiting on message queue receive.\n",0);
			status = msg_q_rcv(msg_q_client_device_io, msg, &msgLen);
			POWERMON_LOGGER(DEV_IO, DEBUG, "Received message of len %d over msg q.\n", msgLen);

			if (status == msg_q_status_success)
			{
				/* msg_src = msg_q_get_client_name(((pwrmon_msg_t *)msg)->src); */

				if (msgLen)
					thread_active = process_received_msg((pwrmon_msg_t *)msg, msgLen);
			}
			else
				POWERMON_LOGGER(DEV_IO, WARN, "Bad return from msg_q_rcv.\n",0);

			if (!thread_active)
				POWERMON_LOGGER(DEV_IO, DEBUG, "device_io thread_active = %d\n", thread_active);

		} while (thread_active == TRUE);

		msg_q_close(msg_q_client_device_io);

	}
	else
		POWERMON_LOGGER(DEV_IO, FATAL, "Unable to create device_io thread message queue.\n",0);

	POWERMON_LOGGER(DEV_IO, THREAD, "Exiting device_io_thread.\n",0);

	if (device_io_data_tid)
	{
		data_thread_active = FALSE;
		pthread_join(device_io_data_tid, (void**)NULL);
	}

	pthread_exit((void *)NULL);

	return (void *)NULL;
}

/* =================================
 * pthread_t get_device_io_tid(void)
 */
pthread_t get_device_io_tid(void)
{
	return device_io_tid;
}

/* =================================
 * void device_io_thread_create(void)
 */

void device_io_thread_create(sem_t *sem)
{
    int err = pthread_create(
    		&device_io_tid,
    		NULL,
			&device_io_thread,
			(void *)sem);

    if (err != 0)
    	POWERMON_LOGGER(DEV_IO, FATAL, "Unable to create device_io_thread :[%s]\n", strerror(err));
}
