/* ============================================================== */
/*
 * msg_queues.h
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

#ifndef __MSG_QUEUES_H__
#define __MSG_QUEUES_H__

#include <mqueue.h>

#define MAX_MESSAGE_SIZE 512
#define MAX_MSG_Q_DEPTH 8

typedef enum
{
	msg_q_client_first = 0,
	msg_q_client_powermon = msg_q_client_first,
	msg_q_client_user_io,
	msg_q_client_console_io,
	msg_q_client_xconsole_io,
	msg_q_client_device_io,
	msg_q_client_data_store,
	msg_q_client_last = msg_q_client_data_store,
	msg_q_client_count,
	msg_q_client_no_client
} msg_q_client_e;

typedef enum
{
	msg_q_status_first = 0,
	msg_q_status_success = msg_q_status_first,
	msg_q_status_already_inservice,
	msg_q_status_no_msg_q,
	msg_q_status_failed_to_send,
	msg_q_status_failed_to_receive,
	msg_q_status_input_bad_params,
	msg_q_status_no_destination,
	msg_q_status_last = msg_q_status_no_destination
} msg_q_status_e;

typedef struct _msg_q
{
	msg_q_client_e client;
	char *client_name;
	mqd_t msg_q;
} msg_q_t;

msg_q_status_e msg_q_init(void);
msg_q_status_e msg_q_open(const msg_q_client_e client);
msg_q_status_e msg_q_close(const msg_q_client_e client);
msg_q_status_e msg_q_send(const msg_q_client_e dst, const char *msg, const unsigned int msgLen);
msg_q_status_e msg_q_rcv(const msg_q_client_e dst, char *msg, unsigned int *msgLen);
void msg_q_dump_client_info(void);
char * msg_q_get_client_name(msg_q_client_e client);


#endif /* __MSG_QUEUES_H__ */
