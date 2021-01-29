/* ============================================================== */
/*
 * msg_queues.c
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

#include <errno.h>

#include "msg_queues.h"
#include "powermon_logger.h"

static msg_q_t queues[msg_q_client_count] = {
	{
		.client = msg_q_client_powermon,
		.client_name = "/powermon",
		.msg_q = -1
	},
	{
		.client = msg_q_client_user_io,
		.client_name = "/user_io",
		.msg_q = -1
	},
	{
		.client = msg_q_client_console_io,
		.client_name = "/console_io",
		.msg_q = -1
	},
	{
		.client = msg_q_client_xconsole_io,
		.client_name = "/xconsole_io",
		.msg_q = -1
	},
	{
		.client = msg_q_client_device_io,
		.client_name = "/device_io",
		.msg_q = -1
	},
	{
		.client = msg_q_client_data_store,
		.client_name = "/data_store",
		.msg_q = -1
	}
};

static const mode_t mode = (S_IWUSR | S_IRUSR | S_ISUID);
static const int flags = (O_RDWR | O_CREAT);

/* =================================
 * msg_q_dump_client_info
 */
void msg_q_dump_client_info(void)
{
	for (msg_q_client_e i = msg_q_client_first; i <= msg_q_client_last; i++)
	{
		printf("Msg Q Client enum: %d\n", queues[i].client);
		printf("Msg Q Client name: %s\n", queues[i].client_name);
		printf("Msg Q Client qid:  %d\n\n", queues[i].msg_q);
	}
}

/* =================================
 * msg_q_get_client_name
 */
char * msg_q_get_client_name(msg_q_client_e client)
{
	char *client_name = NULL;

	switch (client)
	{
	case msg_q_client_powermon:
		client_name = "powermon";
		break;
	case msg_q_client_user_io:
		client_name = "user io";
		break;
	case msg_q_client_console_io:
		client_name = "console io";
		break;
	case msg_q_client_xconsole_io:
		client_name = "xconsole io";
		break;
	case msg_q_client_device_io:
		client_name = "device io";
		break;
	case msg_q_client_data_store:
		client_name = "data store";
		break;
	default:
		POWERMON_LOGGER(MSGQ, WARN, "No client; no name.\n",0);
		break;
	}

	return (client_name);
}

/* =================================
 * msg_q_open
 */
msg_q_status_e msg_q_open(const msg_q_client_e client)
{
	static struct mq_attr attrs;
	msg_q_status_e status = msg_q_status_success;

	if ((queues[client].client == client) && (queues[client].msg_q == -1))
	{
		attrs.mq_maxmsg = MAX_MSG_Q_DEPTH;
		attrs.mq_msgsize = MAX_MESSAGE_SIZE;

		POWERMON_LOGGER(MSGQ, DEBUG, "Opening msg queue for client: %s\n", queues[client].client_name);
		queues[client].msg_q = mq_open(queues[client].client_name, flags, mode, &attrs);

		if (queues[client].msg_q == -1)
		{
			int errsv = errno;
			status = msg_q_status_no_msg_q;
			POWERMON_LOGGER(MSGQ, WARN, "No client message queue (errno = %d).\n", errsv);
		}
	}
	else if (queues[client].msg_q != -1)
	{
		POWERMON_LOGGER(MSGQ, WARN, "Client %s message queue already initialized.\n", msg_q_get_client_name(client));
		status = msg_q_status_already_inservice;
	}
	else /* Association between client enum and client queue is wrong  */
	{
		POWERMON_LOGGER(MSGQ, FATAL, "Client %s message queue inoperable.\n", msg_q_get_client_name(client));
		status = msg_q_status_inoperable;
	}

	return (status);
}

/* =================================
 * msg_q_close
 */
msg_q_status_e msg_q_close(const msg_q_client_e client)
{
	msg_q_status_e status = msg_q_status_success;

	/* Does this client exist in the msg q array */
	/* If so close it; if not return an error */

	if ((queues[client].client == client) && (queues[client].msg_q != -1))
	{
		mq_close(queues[client].msg_q);
		queues[client].msg_q = -1;
	}
	else if (queues[client].client != client)
	{
		POWERMON_LOGGER(MSGQ, FATAL, "Client %s message queue inoperable.\n", msg_q_get_client_name(client));
		status = msg_q_status_inoperable;
	}

	mq_unlink(queues[client].client_name);

	return (status);
}

/* =================================
 * msg_q_send
 */
msg_q_status_e msg_q_send(const msg_q_client_e dst, const char *msg, const unsigned int msgLen)
{
	msg_q_status_e status = msg_q_status_success;

	if (msg && msgLen && msgLen < MAX_MESSAGE_SIZE && queues[dst].msg_q != -1)
	{
		if (mq_send(queues[dst].msg_q, msg, msgLen, 1) == -1)
		{
			int errsv = errno;
			status = msg_q_status_failed_to_send;
			POWERMON_LOGGER(MSGQ, WARN, "Message failed to send, dst: %d, errno: %d\n", dst, errsv);
		}
	}
	else
	{
		status = msg_q_status_input_bad_params;
		POWERMON_LOGGER(MSGQ, WARN, "Bad input parameters: msg: 0x%p, msgLen: %d\n", msg, msgLen);
	}

	return (status);
}

/* =================================
 * msg_q_rcv
 */
msg_q_status_e msg_q_rcv(const msg_q_client_e dst, char *msg, unsigned int *msgLen)
{
	msg_q_status_e status = msg_q_status_success;

	if (msg && msgLen && queues[dst].msg_q != -1)
	{
		*msgLen = mq_receive(queues[dst].msg_q, msg, MAX_MESSAGE_SIZE, NULL);

		if (*msgLen == -1)
		{
			*msgLen = 0;
			int errsv = errno;
			status = msg_q_status_failed_to_receive;
			POWERMON_LOGGER(MSGQ, WARN, "Message failed to receive, dst: %d, errno: %d\n", dst, errsv);
		}
	}
	else
	{
		status = msg_q_status_input_bad_params;
		POWERMON_LOGGER(MSGQ, WARN, "Bad input parameters: msg: 0x%p, msgLen: %d\n", msg, (int)*msgLen);
	}


	return (status);
}
