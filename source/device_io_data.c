/* ============================================================== */
/*
 * device_io_data.c
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

#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <semaphore.h>

#include "common.h"
#include "device_io_data.h"
#include "msg_queues.h"
#include "pwr_mon_msg.h"
#include "powermon_logger.h"
#include "powermon_socket.h"

pthread_t device_io_data_tid = (pthread_t)NULL;

#if 0
/* =================================
 * get/set_device_io_thread_active
 */

static sem_t device_io_data_thread_active_sem;
static unsigned int thread_active = FALSE;

void set_device_io_data_thread_active(unsigned int newVal)
{
	if ((newVal == FALSE) || (newVal == TRUE))
	{
		sem_wait(&device_io_data_thread_active_sem);
		thread_active = newVal;
		sem_post(&device_io_data_thread_active_sem);
	}
}

unsigned int get_device_io_data_thread_active(void)
{
	unsigned int retVal;
	sem_wait(&device_io_data_thread_active_sem);
	retVal = thread_active;
	sem_post(&device_io_data_thread_active_sem);
	return retVal;
}
#endif

/* ========================================*/
/*
 *  decodePowerMonPkt
 */
/* ========================================*/
void decodePowerMonPkt(char *packet, int msgLen)
{
#if 0
	Packet *pkt = (Packet *)packet;

	if (pkt->msgType == client_Data)
	{
		float temp = pkt->data.temp;
		char *mode, *oper;

		printf("Packet data:\n");
		printf("\t  version: %d\n", pkt->version);
		printf("\t  serial#: %015llu-%04d\n", pkt->node.serialNumber.mfgId, pkt->node.serialNumber.nodeId);
		unsigned char *nodeIp = (unsigned char *)&pkt->node.nodeIp;
		printf("\t   nodeIp: %d.%d.%d.%d\n", nodeIp[0], nodeIp[1], nodeIp[2], nodeIp[3]);
		if (pkt->node.mode)
			mode = "Auto";
		else
			mode = "Off";
		printf("\t     mode: %s\n", mode);
		if (pkt->node.operation)
			oper = "Normal";
		else
			oper = "Defrost";
		printf("\toperation: %s\n", oper);
		printf("\t     temp: %.2f\n", temp/100);
		printf("\t     amps: %d\n", pkt->data.amps);
		printf("\n");
	}
#endif
#if 0
	printf("Raw packet data:\n");
	for (int i=0, j=0; i<msgLen; i++, j++)
	{
		printf("%02x ", (unsigned char)packet[i]);

		if (j==7)
			printf("  ");
		else if (j==15)
		{
			printf("\n");
			j=-1;
		}
	}

	printf("\n\n");
#endif
}

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
	msg->src = msg_q_client_device_io;
	msg->version = PWR_MON_MSG_VERSION;
	msg->id = pwr_mon_msg_id_device_io_data;

	POWERMON_LOGGER(DEV_IO, DEBUG, "Sending msg (len: %d) to client %d.\n", msgLen, client);
	status = msg_q_send(client, (char *)msg, msgLen);

	if (status != msg_q_status_success)
		POWERMON_LOGGER(DEV_IO, WARN, "Bad return from msg_q_send.\n",0);
	else
		POWERMON_LOGGER(DEV_IO, DEBUG, "Message sent to %s client.\n", msg_q_get_client_name(client));

	return status;
}

/* ========================================*/
/*
 *  device_io_data_thread
 */
/* ========================================*/
void* device_io_data_thread(void *arg)
{
	unsigned int *threadActive = (unsigned int *)arg;
	msg_q_status_e status;
	unsigned int nodeIp;

	if ((createSocket() == SUCCESS) && (connectSocket() == SUCCESS))
	{
		POWERMON_LOGGER(DEV_IO, THREAD, "device_io_data_thread is alive.\n",0);

		do {
			static pwrmon_msg_t msg;
			unsigned int msgLen;

			memset(&msg, 0, sizeof(pwrmon_msg_t));

			msgLen = svcSocketReceive(&nodeIp, msg.data, sizeof(pwrmon_msg_t));

			POWERMON_LOGGER(DEV_IO, DEBUG, "Packet received.\n",0);

			if (msgLen >= 0)
			{
				((Packet *)msg.data)->node.nodeIp = nodeIp;
				decodePowerMonPkt(msg.data, msgLen);

				/* Send the data to the powermon thread */
				status = send_msg(&msg, msg_q_client_powermon);
				if (status != msg_q_status_success)
				{
					POWERMON_LOGGER(DEV_IO, WARN, "send_msg failed: %d\n",status);
				}
				/* =============================== */
			}

		} while (*threadActive);
	}

	POWERMON_LOGGER(DEV_IO, THREAD, "Exiting device_io_data_thread.\n",0);

	pthread_exit((void *)NULL);

	return (void *)NULL;
}

/* ========================================*/
/*
 *  device_io_data_thread_create
 */
/* ========================================*/
pthread_t device_io_data_thread_create(unsigned int *data_thread_active)
{
    int err = pthread_create(
    		&device_io_data_tid,
    		NULL,
			&device_io_data_thread,
			(void *)data_thread_active);

    if (err != 0)
    {
    	POWERMON_LOGGER(DEV_IO, FATAL, "Unable to create device_io_data_thread :[%s]\n", strerror(err));
    	device_io_data_tid = (pthread_t)NULL;
    }

    return device_io_data_tid;
}
