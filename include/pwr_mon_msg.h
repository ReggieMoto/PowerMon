/* ============================================================== */
/*
 * pwr_mon_msg.h
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

#ifndef __PWR_MON_MSG_H__
#define __PWR_MON_MSG_H__

#include "msg_queues.h"

#define MSG_DATA_SIZE 96
#define PWR_MON_MSG_VERSION 1

typedef enum {
	pwr_mon_msg_id_first,
	pwr_mon_msg_id_kbd_input_str = pwr_mon_msg_id_first,
	pwr_mon_msg_id_credentials,
	pwr_mon_msg_id_device_io_data,
	pwr_mon_msg_id_exit,
	pwr_mon_msg_id_last = pwr_mon_msg_id_exit
} pwr_mon_msg_id_e;

typedef enum {
	pwr_mon_msg_type_first,
	pwr_mon_msg_type_req = pwr_mon_msg_id_first,
	pwr_mon_msg_type_rsp,
	pwr_mon_msg_type_info,
	pwr_mon_msg_type_last = pwr_mon_msg_type_info
} pwr_mon_msg_type_e;

typedef struct msg_v1 {
	/* Message */
	msg_q_client_e src;
	pwr_mon_msg_id_e id;
	pwr_mon_msg_type_e type;
	char data[MSG_DATA_SIZE];
	/* Versioning */
	unsigned int version;
	void *nextVersion;
} msg_v1_t, pwrmon_msg_t;

#endif /* __PWR_MON_MSG_H__ */

