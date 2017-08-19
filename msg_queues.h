/*=======================================
 * msg_queues.h
 */

#include <mqueue.h>

#ifndef __MSG_QUEUES_H__
#define __MSG_QUEUES_H__

#define MAX_MESSAGE_SIZE 512

extern mqd_t pwr_mon_mq;
extern mqd_t user_io_mq;


#endif /* __MSG_QUEUES_H__ */
