/* =================================
 * main.c
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <mqueue.h>

#include "common.h"
#include "msg_queues.h"

extern void* powermon_thread_create(void);
extern void* user_io_thread_create(void);
extern void* wifi_io_thread_create(void);
extern void* data_store_thread_create(void);

extern pthread_t wifi_io_tid;
extern pthread_t user_io_tid;
extern pthread_t data_store_tid;
extern pthread_t powermon_tid;


int main(int argc, char *argv[])
{
	printf ("\nPowerMon starting up.\n\n");

	printf ("Spawning the powermon_thread.\n");
	powermon_thread_create();

	printf ("Spawning the user_io_thread.\n");
	user_io_thread_create();

	printf ("Spawning the wifi_io_thread.\n");
	wifi_io_thread_create();

	printf ("Spawning the data_store_thread.\n");
	data_store_thread_create();

	/* Now run the power monitor thread */
	//power_mon_thread(pwr_mon_mq);

	pthread_join(powermon_tid, (void**)NULL);
	pthread_join(user_io_tid, (void**)NULL);
	pthread_join(wifi_io_tid, (void**)NULL);
	pthread_join(data_store_tid, (void**)NULL);

	printf("\nAll PowerMon threads terminated.\n\n");

	return 0;
}
