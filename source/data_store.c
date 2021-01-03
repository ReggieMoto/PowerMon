/* ============================================================== */
/*
 * data_store.c
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
#include "sha3c.h"
#include "device_io_data.h"
#include "powermon_accts.h"

static pthread_t data_store_tid = (pthread_t)NULL;

static unsigned char default_password[] = { /* admin */
	0xfb, 0x00, 0x1d, 0xfc, 0xff, 0xd1, 0xc8, 0x99, 0xf3, 0x29, 0x78, 0x71, 0x40, 0x62, 0x42, 0xf0,
	0x97, 0xae, 0xcf, 0x1a, 0x53, 0x42, 0xcc, 0xf3, 0xeb, 0xcd, 0x11, 0x61, 0x46, 0x18, 0x8e, 0x4b,
	0x9d, 0x69, 0x42, 0x27, 0x24, 0xea, 0x45, 0xea, 0xc3, 0x20, 0x2a, 0x3d, 0x5a, 0x7a, 0x98, 0x20,
	0x22, 0x02, 0x44, 0xad, 0x1f, 0xb5, 0x1c, 0x46, 0x7c, 0x4d, 0xf0, 0xde, 0xb4, 0xde, 0x08, 0xdb,
	0x4d, 0x4b, 0x87, 0xd9, 0x13, 0x77, 0xf9, 0x6e, 0x27, 0x6b, 0xe9, 0x83, 0xe6, 0xbb, 0x83, 0xe0,
	0x96, 0xf8, 0xe6, 0xf2, 0xeb, 0x21, 0x50, 0x8e, 0x7a, 0xd9, 0x65, 0x79, 0x39, 0x7b, 0xe5, 0x5f,
	0x94, 0x72, 0x95, 0x18, 0x81, 0xd1, 0x89, 0x2e, 0xe4, 0x0c, 0xde, 0x89, 0xef, 0xb3, 0x45, 0xb5,
	0x31, 0xe5, 0x31, 0xfe, 0xef, 0x63, 0xd4, 0xcb, 0xa2, 0xd0, 0x9c, 0xe0, 0xbd, 0x04, 0x02, 0x9e,
	0x55, 0x18, 0x8f, 0x41, 0xd2, 0x40, 0x35, 0x42, 0xa1, 0x67, 0xfd, 0x01, 0x9e, 0x13, 0xbf, 0x25,
	0x9a, 0x60, 0x48, 0x0e, 0xb2, 0xb1, 0xf1, 0xfc, 0xe1, 0xa7, 0x0f, 0xcb, 0x84, 0xb1, 0x67, 0xe6,
	0x8c, 0xca, 0xb0, 0x9f, 0x4a, 0xd4, 0xfe, 0x05, 0x08, 0xd0, 0x53, 0xc7, 0xb0, 0xd2, 0x65, 0x0d,
	0x8d, 0x70, 0xa8, 0xac, 0x58, 0x7f, 0x62, 0xfc, 0x25, 0xe6, 0xda, 0x5e, 0xca, 0xb2, 0xc8, 0x71,
	0x87, 0x22, 0xe1, 0xa4, 0x03, 0xc5, 0x04, 0x85
};

static LoginAccounts loginAccounts;
static AccountType currentUser;

/* =================================
 * send_msg
 */
static msg_q_status_e send_msg(pwrmon_msg_t *msg, msg_q_client_e client)
{
	unsigned int msgLen = sizeof(pwrmon_msg_t);
	msg_q_status_e status;

	/* Prepare the msg q header */
	msg->src = msg_q_client_data_store;
	msg->version = PWR_MON_MSG_VERSION;

	POWERMON_LOGGER(DSTORE, DEBUG, "Sending msg (len: %d) to client %d.\n", msgLen, client);
	status = msg_q_send(client, (char *)msg, msgLen);

	if (status != msg_q_status_success)
		POWERMON_LOGGER(DSTORE, WARN, "Bad return from msg_q_send.\n",0);
	else
		POWERMON_LOGGER(DSTORE, DEBUG, "Message sent to %s client.\n", msg_q_get_client_name(client));

	return status;
}

/* =================================
 * validate_login_credentials
 */
static unsigned int validate_login_credentials(char *username, unsigned char *hash)
{

	unsigned int login_valid = FALSE;

	POWERMON_LOGGER(DSTORE, DEBUG, "SHA3_KECCAK_SPONGE_WORDS: %lu\n", SHA3_KECCAK_SPONGE_WORDS);
	POWERMON_LOGGER(DSTORE, DEBUG, "sizeof(hash[SHA3_KECCAK_SPONGE_WORDS*8]): %lu\n", SHA3_KECCAK_SPONGE_WORDS*8);

	for (int i=0; i<MAX_LOGIN_ACCOUNTS; i++)
	{
		POWERMON_LOGGER(DSTORE, TRACE, "username: %s, len: %d\n", username, strlen(username));
		POWERMON_LOGGER(DSTORE, TRACE, "account[%d].user: %s, len: %d\n",
				i, loginAccounts.account[i].user, strlen(loginAccounts.account[i].user));

		if ((strlen(loginAccounts.account[i].user) == strlen(username)) &&
			(strcmp(loginAccounts.account[i].user, username) == 0) &&
			(memcmp(loginAccounts.account[i].hash, hash, SHA3_HASH_LEN) == 0))
		{
			login_valid = TRUE;
			break;
		}
	}

#if 0
	printf("username: %s\n", username);
	printf("hash:\t");
	for(int i=0, j=0; i<SHA3_KECCAK_SPONGE_WORDS*8; i++, j++)
	{

		printf("%02x", (unsigned char)*(hash+i));
		if (j==7) printf(" ");
		else if (j==15) { j=(-1); printf("\n\t"); }
	}
	printf("\n");
#endif

	return login_valid;
}

/* =================================
 * process_login_credentials
 */

static unsigned int process_login_credentials(credentials_t *credentials)
{
	unsigned int login_valid = TRUE;
	sha3_context ctxt;
	uint8_t *hash;

	memset(&ctxt, 0, sizeof(sha3_context));

    sha3_Init256(&ctxt);
    sha3_Update(&ctxt, credentials->password, strlen(credentials->password));
    hash = (uint8_t *)sha3_Finalize(&ctxt);
    // 'hash' points to a buffer inside 'ctxt'
    // with the value of SHA3-256

    login_valid = validate_login_credentials(credentials->username, hash);

	return login_valid;
}

/* =================================
 * process_received_msg
 */

static unsigned int process_received_msg(pwrmon_msg_t *msg, const char msgLen)
{
	unsigned int thread_active = TRUE;
	unsigned int login_valid;
	char *msg_src = msg_q_get_client_name(msg->src);

	switch(msg->id) {

	case pwr_mon_msg_id_credentials:

		POWERMON_LOGGER(DSTORE, DEBUG, "Received msg from %s client containing login credentials.\n", msg_src);

		credentials_t *credentials = (credentials_t *)msg->data;
		POWERMON_LOGGER(DSTORE, DEBUG, "%s: username: %s\n", __FUNCTION__, credentials->username);
		POWERMON_LOGGER(DSTORE, DEBUG, "%s: password: %s\n", __FUNCTION__, credentials->password);

		login_valid = process_login_credentials(credentials);

		if (login_valid)
			credentials->valid = pwr_mon_credentials_valid;
		else
			credentials->valid = pwr_mon_credentials_invalid;

		msg->type = pwr_mon_msg_type_rsp;
		send_msg(msg, msg->src);

		break;

	case pwr_mon_msg_id_device_io_data:
		break;

	case pwr_mon_msg_id_exit:
		POWERMON_LOGGER(DSTORE, DEBUG, "Received msg from %s client to exit thread.\n", msg_src);
		thread_active = FALSE;
		break;

	default:
		thread_active = TRUE;
		break;
	}

	return thread_active;
}

void resetDefaultAccount(void)
{
	static const char *defaultAdminUser = "admin";

	memset(&loginAccounts, 0, sizeof(LoginAccounts));
	strncpy(loginAccounts.account[0].user, defaultAdminUser, USER_NAME_LEN);
	loginAccounts.account[0].type = none;
	memcpy(loginAccounts.account[0].hash, default_password, SHA3_HASH_LEN);

	currentUser = none;
}

/* =================================
 * void data_store_thread(void)
 */

void* data_store_thread(void *arg)
{
	sem_t *sem = (sem_t *)arg;
	msg_q_status_e status;
	unsigned int thread_active = TRUE;

	/* Register the data_store_io message queue */
	status = msg_q_open(msg_q_client_data_store);

	if (status == msg_q_status_success)
	{
		sem_wait(sem);
		POWERMON_LOGGER(DSTORE, THREAD, "data_store_thread is alive.\n",0);

		resetDefaultAccount();

		do {
			static char msg[MAX_MESSAGE_SIZE];
			unsigned int msgLen = MAX_MESSAGE_SIZE;

			memset(msg, 0, msgLen);

			POWERMON_LOGGER(DSTORE, DEBUG, "Waiting on message queue receive.\n",0);
			status = msg_q_rcv(msg_q_client_data_store, msg, &msgLen);
			POWERMON_LOGGER(DSTORE, DEBUG, "Received message of len %d over msg q.\n", msgLen);

			if (status == msg_q_status_success)
			{
				if (msgLen)
					thread_active = process_received_msg((pwrmon_msg_t *)msg, msgLen);
			}
			else
				POWERMON_LOGGER(DSTORE, WARN, "Bad return from msg_q_rcv.\n",0);

			if (!thread_active)
				POWERMON_LOGGER(DSTORE, DEBUG, "data_store thread_active = %d\n", thread_active);

		} while (thread_active == TRUE);

		msg_q_close(msg_q_client_data_store);

	}
	else
		POWERMON_LOGGER(DSTORE, FATAL, "Unable to create data_store thread message queue.\n",0);

	POWERMON_LOGGER(DSTORE, THREAD, "Exiting data_store thread.\n",0);
	pthread_exit((void *)NULL);

	return (void *)NULL;
}

/* =================================
 * pthread_t get_data_store_tid(void)
 */
pthread_t get_data_store_tid(void)
{
	return data_store_tid;
}

/* =================================
 * void data_store_thread_create(void)
 */
void data_store_thread_create(sem_t *sem)
{
    int err = pthread_create(
    		&data_store_tid,
    		NULL,
			&data_store_thread,
			(void *)sem);

    if (err != 0)
    	POWERMON_LOGGER(DSTORE, FATAL, "Unable to create data_store_thread :[%s]\n", strerror(err));
}
