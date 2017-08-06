/* =================================
 * data-store.c
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

pthread_t data_store_tid = (pthread_t)NULL;

/* =================================
 * void data_store_thread(void)
 */

void* data_store_thread(void *arg)
{
	printf("\tPowerMon: data_store_thread.\n");
	pthread_exit((void *)NULL);

	return (void *)NULL;
}

/* =================================
 * void data_store_thread_create(void)
 */
void data_store_thread_create(void)
{
    int err = pthread_create(
    		&data_store_tid,
    		NULL,
			&data_store_thread,
			NULL);

    if (err != 0)
        printf("\tUnable to create data_store_thread :[%s]\n", strerror(err));
}
