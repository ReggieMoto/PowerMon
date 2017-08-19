/* =================================
 * powermon.c
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "common.h"
#include "msg_queues.h"

pthread_t powermon_tid = (pthread_t)NULL;

/* =================================
 * void powermon_thread(void)
 */

void* powermon_thread(void *powermon_mq)
{
	printf("\tPowerMon: powermon_thread.\n");
	pthread_exit((void *)NULL);

	return (void *)NULL;
}

/* =================================
 * void powermon_thread_create(void)
 */
void powermon_thread_create(void)
{
	int err = pthread_create(
			&powermon_tid,
			NULL,
			&powermon_thread,
			NULL);

	if (err != 0)
		printf("\tUnable to create powermon_thread :[%s]\n", strerror(err));
}
