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
#include <semaphore.h>

#include "common.h"
#include "msg_queues.h"
#include "pwr_mon_msg.h"
#include "powermon_logger.h"
#include "credentials.h"
#include "user_io_fsm.h"

extern pthread_t console_io_thread_create(unsigned int *console_io_thread_active);
extern void user_io_fsm_process_string(pwrmon_msg_t *msg);

extern sem_t powermon_login_sem;
extern unsigned int login_successful;

static pthread_t user_io_tid = (pthread_t)NULL;

/* =================================
 * send_msg
 */

static pwrmon_msg_t msg;

static msg_q_status_e send_msg(char *buffer, int buf_len, pwr_mon_msg_id_e id, msg_q_client_e client)
{
	msg_q_status_e status;
	char *msg_dst = msg_q_get_client_name(client);
	unsigned int msgLen = sizeof(pwrmon_msg_t);

	/* Prepare the msg q header */
	msg.src = msg_q_client_user_io;
	msg.id = id;
	msg.version = PWR_MON_MSG_VERSION;

	if (buffer && buf_len)
		memcpy(msg.data, buffer, buf_len);

	POWERMON_LOGGER(USER_IO, DEBUG, "Sending msg (len: %d) to %s client.\n", msgLen, msg_dst);
	status = msg_q_send(client, (char *)&msg, msgLen);

	if (status != msg_q_status_success)
		POWERMON_LOGGER(USER_IO, FATAL, "Bad return from msg_q_rcv.\n",0);
	else
		POWERMON_LOGGER(USER_IO, DEBUG, "Message sent to %s client.\n", msg_dst);

	return status;
}

void user_io_send_exit_msg(void)
{
	send_msg(NULL, 0, pwr_mon_msg_id_exit, msg_q_client_powermon);
}

void user_io_send_credentials(credentials_t *credentials)
{
	send_msg((char *)credentials, sizeof(credentials_t), pwr_mon_msg_id_credentials, msg_q_client_data_store);
}

/* =================================
 * process_keypress
 */
static void process_keypress(pwrmon_msg_t *msg)
{
	switch(msg->data[0]) {

	case 'b':
	case 'B':
		POWERMON_LOGGER(USER_IO, DEBUG, "Received user keypress 'b'.\n", 0);
		user_io_fsm(user_io_input_key_b);
		break;

	case 'c':
	case 'C':
		POWERMON_LOGGER(USER_IO, DEBUG, "Received user keypress 'c'.\n", 0);
		user_io_fsm(user_io_input_key_c);
		break;

	case 'd':
	case 'D':
		POWERMON_LOGGER(USER_IO, DEBUG, "Received user keypress 'd'.\n", 0);
		user_io_fsm(user_io_input_key_d);
		break;

	case 'l':
	case 'L':
		POWERMON_LOGGER(USER_IO, DEBUG, "Received user keypress 'l'.\n", 0);
		user_io_fsm(user_io_input_key_l);
		break;

	case 'm':
	case 'M':
		POWERMON_LOGGER(USER_IO, DEBUG, "Received user keypress 'm'.\n", 0);
		user_io_fsm(user_io_input_key_m);
		break;

	case 'n':
	case 'N':
		POWERMON_LOGGER(USER_IO, DEBUG, "Received user keypress 'n'.\n", 0);
		user_io_fsm(user_io_input_key_n);
		break;

	case 'o':
	case 'O':
		POWERMON_LOGGER(USER_IO, DEBUG, "Received user keypress 'o'.\n", 0);
		user_io_fsm(user_io_input_key_o);
		break;

	case 'r':
	case 'R':
		POWERMON_LOGGER(USER_IO, DEBUG, "Received user keypress 'r'.\n", 0);
		user_io_fsm(user_io_input_key_r);
		break;

	case 't':
	case 'T':
		POWERMON_LOGGER(USER_IO, DEBUG, "Received user keypress 't'.\n", 0);
		user_io_fsm(user_io_input_key_t);
		break;

	case 'u':
	case 'U':
		POWERMON_LOGGER(USER_IO, DEBUG, "Received user keypress 'u'.\n", 0);
		user_io_fsm(user_io_input_key_u);
		break;

	case 'x':
	case 'X':
		POWERMON_LOGGER(USER_IO, DEBUG, "Received user keypress 'x'.\n", 0);
		user_io_fsm(user_io_input_key_x);
		break;

	default:
		POWERMON_LOGGER(USER_IO, DEBUG, "Received unhandled keypress (ignored).\n", 0);
		break;
	}
}

/* =================================
 * process_received_msg
 */

static unsigned int process_received_msg(pwrmon_msg_t *msg, const char msgLen)
{
	unsigned int thread_active = TRUE;
	msg_q_client_e src = msg->src;
	char *msg_src = msg_q_get_client_name(src);
	credentials_t *credentials;

	switch(msg->id) {

	case pwr_mon_msg_id_exit:
		POWERMON_LOGGER(USER_IO, DEBUG, "Received msg from %s client to exit thread.\n", msg_src);
		thread_active = FALSE;
		break;

	case pwr_mon_msg_id_credentials:
		credentials = (credentials_t *)msg->data;
		if (credentials->valid == pwr_mon_credentials_valid)
			user_io_fsm(user_io_input_login_valid);
		else
			user_io_fsm(user_io_input_login_invalid);

		break;

	case pwr_mon_msg_id_kbd_input_str:
		if (strlen(msg->data) == 1)
		{
			POWERMON_LOGGER(USER_IO, DEBUG, "Received keypress from %s client.\n", msg_src);
			process_keypress(msg);
		}
		else if (strlen(msg->data) > 1)
		{
			POWERMON_LOGGER(USER_IO, DEBUG, "Received input string from %s client.\n", msg_src);
			user_io_fsm_process_string(msg);
		}
		else
			POWERMON_LOGGER(USER_IO, DEBUG, "Received empty data message from %s client.\n", msg_src);

		break;

	default:
		POWERMON_LOGGER(USER_IO, WARN, "Received unknown message from %s client.\n", msg_src);
		break;
	}

	return thread_active;
}

#if 0
static void handlerUserIdleTimer(int sig, siginfo_t *si, void *uc)
{

}
#endif
/* =================================
 * user_io_thread
 */

void* user_io_thread(void *arg)
{
	pthread_t console_io_tid = (pthread_t)NULL;
	unsigned int thread_active = TRUE;
	unsigned int console_thread_active = TRUE;

	msg_q_status_e status;
	sem_t *sem = (sem_t *)arg;

	/* Register the user I/O message queue */
	status = msg_q_open(msg_q_client_user_io);

	if (status == msg_q_status_success)
	{
		sem_wait(sem);
		POWERMON_LOGGER(USER_IO, THREAD, "%s is alive.\n", __FUNCTION__);
		console_io_tid = console_io_thread_create(&console_thread_active);
		user_io_fsm(user_io_input_login_start);

		do {
			static char msg[MAX_MESSAGE_SIZE];
			unsigned int msgLen = MAX_MESSAGE_SIZE;

			/* Wait on message queue */
			memset(msg, 0, msgLen);

			POWERMON_LOGGER(USER_IO, DEBUG, "Waiting on message queue receive.\n",0);
			status = msg_q_rcv(msg_q_client_user_io, msg, &msgLen);
			POWERMON_LOGGER(USER_IO, DEBUG, "Received message of len %d.\n", msgLen);

			if (status == msg_q_status_success)
			{
				if (msgLen)
					thread_active = process_received_msg((pwrmon_msg_t *)msg, msgLen);
			}
			else
				POWERMON_LOGGER(USER_IO, WARN, "Bad return from msg_q_rcv.\n",0);

			if (!thread_active)
				POWERMON_LOGGER(USER_IO, DEBUG, "user_io thread_active = %d\n", thread_active);

		} while (thread_active == TRUE);

		msg_q_close(msg_q_client_user_io);

	}
	else
		POWERMON_LOGGER(USER_IO, FATAL, "Unable to create User I/O thread message queue.\n",0);

	POWERMON_LOGGER(USER_IO, THREAD, "Exiting %s.\n", __FUNCTION__);

	if (console_io_tid)
	{
		console_thread_active = FALSE;
		pthread_join(console_io_tid, (void**)NULL);
	}

	pthread_exit((void *)NULL);

	return (void *)NULL;
}

/* =================================
 * pthread_t get_user_io_tid(void)
 */
pthread_t get_user_io_tid(void)
{
	return user_io_tid;
}

/* =================================
 * user_io_thread_create
 */
void user_io_thread_create(sem_t *sem)
{
	int err = pthread_create(
			&user_io_tid,
			NULL,
			&user_io_thread,
			(void *)sem);

	if (err != 0)
		POWERMON_LOGGER(USER_IO, FATAL, "Unable to create user_io_thread :[%s]\n", strerror(err));
}
