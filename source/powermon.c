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
#include <semaphore.h>

#include "common.h"
#include "msg_queues.h"
#include "pwr_mon_msg.h"
#include "powermon_logger.h"
#include "credentials.h"
#include "device_io_data.h"
#include "powermon_svc.h"

static pthread_t powermon_tid = (pthread_t)NULL;

void addToPollCycleData(Packet *);
pthread_t powermon_calc_thread_create(unsigned int *);

/* ========================================*/
/*
 *  send_msg
 */
/* ========================================*/
static msg_q_status_e send_msg(pwrmon_msg_t *msg, msg_q_client_e client)
{
	unsigned int msgLen = sizeof(pwrmon_msg_t);
	msg_q_status_e status;

	/* Prepare the msg q header */
	msg->src = msg_q_client_powermon;
	msg->version = PWR_MON_MSG_VERSION;

	POWERMON_LOGGER(PWRMON, DEBUG, "Sending msg (len: %d) to client %s.\n", msgLen, msg_q_get_client_name(client));
	status = msg_q_send(client, (char *)msg, msgLen);

	if (status != msg_q_status_success)
		POWERMON_LOGGER(PWRMON, WARN, "Bad return from msg_q_send.\n",0);
	else
		POWERMON_LOGGER(PWRMON, DEBUG, "Message sent to %s client.\n", msg_q_get_client_name(client));

	return (status);
}

/* ========================================*/
/*
 *  process_received_msg
 */
/* ========================================*/
static unsigned int process_received_msg(pwrmon_msg_t *msg, const char msgLen)
{
	unsigned int thread_active = TRUE;
	msg_q_client_e src = msg->src;
	msg_q_status_e status;
	char *msg_src = msg_q_get_client_name(src);

	switch(msg->id) {

	case pwr_mon_msg_id_credentials:

		POWERMON_LOGGER(PWRMON, DEBUG, "Received user request from %s client to validate credentials.\n", msg_src);

		POWERMON_LOGGER(PWRMON, DEBUG, "username: %s\n", get_username());
		POWERMON_LOGGER(PWRMON, DEBUG, "password: %s\n", get_password());

		status = send_msg(msg, msg_q_client_data_store);
		if (status != msg_q_status_success)
			POWERMON_LOGGER(PWRMON, WARN, "Unable to send message to %s client.\n", msg_src);

		break;

	case pwr_mon_msg_id_device_io_data:

		POWERMON_LOGGER(PWRMON, DEBUG, "Received user request from %s client containing power data.\n", msg_src);
		addToPollCycleData((Packet *)(msg->data));
		break;

	case pwr_mon_msg_id_exit:

		POWERMON_LOGGER(PWRMON, DEBUG, "Received user request from %s client to terminate PowerMon.\n", msg_src);

		for (msg_q_client_e client = msg_q_client_first; client <= msg_q_client_last; client++)
		{
			if ((client != msg_q_client_powermon) &&
				(client != msg_q_client_console_io))
			{
				status = send_msg(msg, client);
				if (status != msg_q_status_success)
					POWERMON_LOGGER(PWRMON, WARN, "Unable to send exit message to %s client.\n", msg_q_get_client_name(client));
			}
		}

		POWERMON_LOGGER(PWRMON, DEBUG, "Signal the AVAHI service thread to terminate.\n", msg_src);
		set_avahi_thread_inactive();
		thread_active = FALSE;
		break;

	default:
		POWERMON_LOGGER(PWRMON, WARN, "Received unhandled message from %s client.\n", msg_src);
		thread_active = TRUE;
		break;
	}

	return (thread_active);
}

/* ========================================*/
/*
 *  powermon_thread
 */
/* ========================================*/
void* powermon_thread(void *arg)
{
	pthread_t powermon_calc_tid = (pthread_t)NULL;
	unsigned int thread_active = TRUE;
	unsigned int calc_thread_active = TRUE;

	msg_q_status_e status;
	sem_t *sem = (sem_t *)arg;

	/* Register the powermon message queue */
	status = msg_q_open(msg_q_client_powermon);

	if (status == msg_q_status_success)
	{
		sem_wait(sem);
		POWERMON_LOGGER(PWRMON, THREAD, "powermon_thread is alive.\n",0);

		powermon_calc_tid = powermon_calc_thread_create(&calc_thread_active);

		do {
			static char msg[MAX_MESSAGE_SIZE];
			unsigned int msgLen = MAX_MESSAGE_SIZE;

			memset(msg, 0, msgLen);

			POWERMON_LOGGER(PWRMON, DEBUG, "Waiting on message queue receive.\n",0);
			status = msg_q_rcv(msg_q_client_powermon, msg, &msgLen);
			POWERMON_LOGGER(PWRMON, DEBUG, "Received message of len %d over msg q.\n", msgLen);

			if (status == msg_q_status_success)
			{
				if (msgLen)
					thread_active = process_received_msg((pwrmon_msg_t *)msg, msgLen);
			}
			else
				POWERMON_LOGGER(PWRMON, WARN, "Bad return from msg_q_rcv.\n",0);

			if (!thread_active)
				POWERMON_LOGGER(PWRMON, DEBUG, "powermon thread_active = %d\n", thread_active);

		} while (thread_active == TRUE);

		msg_q_close(msg_q_client_powermon);

	}
	else
		POWERMON_LOGGER(PWRMON, FATAL, "Unable to create powermon thread message queue.\n",0);

	POWERMON_LOGGER(PWRMON, THREAD, "Exiting powermon thread.\n",0);

	if (powermon_calc_tid)
	{
		calc_thread_active = FALSE;
		pthread_join(powermon_calc_tid, (void**)NULL);
	}

	pthread_exit((void *)NULL);

	return ((void *)NULL);
}

/* =================================
 * pthread_t get_powermon_tid(void)
 */
pthread_t get_powermon_tid(void)
{
	return (powermon_tid);
}

/* ========================================*/
/*
 *  powermon_thread_create
 */
/* ========================================*/
void powermon_thread_create(sem_t *sem)
{
	int err = pthread_create(
			&powermon_tid,
			NULL,
			&powermon_thread,
			(void *)sem);

	if (err != 0)
		POWERMON_LOGGER(PWRMON, FATAL, "Unable to create powermon_thread :[%s]\n", strerror(err));
}
