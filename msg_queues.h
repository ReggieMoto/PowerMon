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

#include <mqueue.h>

#ifndef __MSG_QUEUES_H__
#define __MSG_QUEUES_H__

#define MAX_MESSAGE_SIZE 512

extern mqd_t pwr_mon_mq;
extern mqd_t user_io_mq;


#endif /* __MSG_QUEUES_H__ */
