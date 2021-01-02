/* ============================================================== */
/*
 * main.c
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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <mqueue.h>
#include <semaphore.h>

#include "common.h"
#include "msg_queues.h"
#include "powermon_logger.h"
#include "powermon_time.h"
#include "powermon_svc.h"
#include "powermon.h"
#include "user_io.h"
#include "device_io.h"
#include "data_store.h"

void sighandler(int signum)
{
	printf("Process %d got signal %d\n", getpid(), signum);
	signal(signum, SIG_DFL);
	kill(getpid(), signum);
}

int main(int argc, char *argv[])
{
	sem_t avahi_svc_sem;
	sem_t powermon_sem;
	sem_t user_io_sem;
	sem_t device_io_sem;
	sem_t data_store_sem;

	signal(SIGSEGV, sighandler);
	time_t tm = getPwrmonSystemStartTime();

#if 0
	/* Create working directories */
	struct stat st = {0};

	/* log */
	if ((stat("/home/PowerMon/log", &st) == -1) && (errno == ENOENT))
		if (mkdir("/home/PowerMon/log", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
		{
			printf("Unable to create log directory in home directory.\n\n");
			exit(1);
		}

	/* core */
	if ((stat("/home/PowerMon/core", &st) == -1) && (errno == ENOENT))
		if (mkdir("/home/PowerMon/core", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
		{
			printf("Unable to create core directory in home directory.\n\n");
			exit(1);
		}
#endif

	/* Initialize system semaphores */
	sem_init(&avahi_svc_sem, 0, 0);
	sem_init(&powermon_sem, 0, 0);
	sem_init(&user_io_sem, 0, 0);
	sem_init(&device_io_sem, 0, 0);
	sem_init(&data_store_sem, 0, 0);

	msg_q_init();
	powermon_logger_init();

	POWERMON_LOGGER(MAIN, INFO, "PowerMon starting up at %s.\n\n", ctime(&tm));

	POWERMON_LOGGER(MAIN, THREAD, "Spawning the avahi_svc_thread.\n",0);
	avahi_svc_thread_create(&avahi_svc_sem);

	POWERMON_LOGGER(MAIN, THREAD, "Spawning the powermon_thread.\n",0);
	powermon_thread_create(&powermon_sem);

	POWERMON_LOGGER(MAIN, THREAD, "Spawning the user_io_thread.\n",0);
	user_io_thread_create(&user_io_sem);

	POWERMON_LOGGER(MAIN, THREAD, "Spawning the device_io_thread.\n",0);
	device_io_thread_create(&device_io_sem);

	POWERMON_LOGGER(MAIN, THREAD, "Spawning the data_store_thread.\n",0);
	data_store_thread_create(&data_store_sem);

	/* Release the threads */
	sem_post(&avahi_svc_sem);
	sem_post(&powermon_sem);
	sem_post(&user_io_sem);
	sem_post(&device_io_sem);
	sem_post(&data_store_sem);

	/* Wait here to join all active threads upon exit */
	pthread_join(get_avahi_svc_tid(), (void**)NULL);
	pthread_join(get_powermon_tid(), (void**)NULL);
	pthread_join(get_user_io_tid(), (void**)NULL);
	pthread_join(get_device_io_tid(), (void**)NULL);
	pthread_join(get_data_store_tid(), (void**)NULL);

	POWERMON_LOGGER(MAIN, THREAD, "All PowerMon threads terminated.\n\n",0);

	return 0;
}
