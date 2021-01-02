/* ============================================================== */
/*
 * console_io.c
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

pthread_t console_io_tid = (pthread_t)NULL;

sem_t powermon_login_sem;
unsigned int login_successful = FALSE;

extern credentials_t * powermon_login(void);

#if 0
/* =================================
 * get/set_console_io_thread_active
 */

static sem_t console_io_thread_active_sem;
static unsigned int thread_active= FALSE;

void set_console_io_thread_active(unsigned int newVal)
{
	if ((newVal == FALSE) || (newVal == TRUE))
	{
		sem_wait(&console_io_thread_active_sem);
		thread_active = newVal;
		sem_post(&console_io_thread_active_sem);
	}
}

unsigned int get_console_io_thread_active(void)
{
	unsigned int retVal;
	sem_wait(&console_io_thread_active_sem);
	retVal = thread_active;
	sem_post(&console_io_thread_active_sem);
	return retVal;
}
#endif

/* =================================
 * send_msg
 */
static msg_q_status_e send_msg(char *message, pwr_mon_msg_id_e msg_id)
{
	msg_q_status_e status;
	unsigned int console_msg_len;
	pwrmon_msg_t msg;
	unsigned int msgLen = sizeof(pwrmon_msg_t);

	memset((char *)&msg, 0, msgLen);

	/* Prepare the msg q header */
	msg.src = msg_q_client_console_io;
	msg.version = PWR_MON_MSG_VERSION;
	msg.id = msg_id;
	msg.type = pwr_mon_msg_type_info;

	if (strlen(message) < MSG_DATA_SIZE)
		console_msg_len = strlen(message);
	else
		console_msg_len = MSG_DATA_SIZE;

	memcpy(msg.data, message, console_msg_len);

	POWERMON_LOGGER(CONSOLE_IO, DEBUG, "Sending msg (len: %d) to client %d.\n", msgLen, msg_q_client_user_io);

	status = msg_q_send(msg_q_client_user_io, (char *)&msg, msgLen);

	if (status != msg_q_status_success)
		POWERMON_LOGGER(CONSOLE_IO, FATAL, "Bad return from msg_q_rcv.\n",0);
	else
		POWERMON_LOGGER(CONSOLE_IO, INFO, "Message sent to user io client.\n",0);

	return status;
}

/* =================================
 * console_read_kbd
 */
char * console_read_kbd(void)
{
	char keypress = 0x00;
	static char strBuffer[MAX_KBD_INPUT_STR_LEN];
	unsigned int strIndex = 0;

	memset(strBuffer, 0, MAX_KBD_INPUT_STR_LEN);

	do {
		keypress = fgetc(stdin);

		POWERMON_LOGGER(CONSOLE_IO, DEBUG, "Received keypress %d: 0x%2x.\n", strIndex, keypress);

		if ((strIndex == MAX_KBD_INPUT_STR_LEN-1) || (keypress == 0x0a))
			strBuffer[strIndex++] = 0x00;
		else
			strBuffer[strIndex++] = keypress;

	} while ((strIndex < MAX_KBD_INPUT_STR_LEN) && (keypress != 0x0a));

	POWERMON_LOGGER(CONSOLE_IO, DEBUG, "string: \"%s\"; length: %d\n", strBuffer, strlen(strBuffer));

	return strBuffer;
}

/* =================================
 * console_io_thread
 */
void* console_io_thread(void *arg)
{
	unsigned int *threadActive = (unsigned int *)arg;

	POWERMON_LOGGER(CONSOLE_IO, THREAD, "%s is alive.\n", __FUNCTION__);

	/* Running */
	do {
		char *kbd_input_str;

		/* Wait on keyboard input */
		POWERMON_LOGGER(CONSOLE_IO, INFO, "Waiting to receive console input.\n", 0);

		kbd_input_str = console_read_kbd();

		if (strlen(kbd_input_str))
		{
			POWERMON_LOGGER(CONSOLE_IO, DEBUG, "Console input received.\n", 0);
			send_msg(kbd_input_str, pwr_mon_msg_id_kbd_input_str);
		}
		else
			POWERMON_LOGGER(CONSOLE_IO, DEBUG, "Empty console string received.\n", 0);

	} while (*threadActive);

	POWERMON_LOGGER(CONSOLE_IO, THREAD, "Exiting %s.\n", __FUNCTION__);
	pthread_exit((void *)NULL);

	return (void *)NULL;
}

/* =================================
 * console_io_thread_create
 */
pthread_t console_io_thread_create(unsigned int *console_io_thread_active)
{
	int err = pthread_create(
			&console_io_tid,
			NULL,
			&console_io_thread,
			(void *)console_io_thread_active);

	if (err != 0)
	{
		POWERMON_LOGGER(USER_IO, FATAL, "Unable to create console_io_thread :[%s]\n", strerror(err));
	}

	return console_io_tid;
}
