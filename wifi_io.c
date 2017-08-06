/* =================================
 * wifi_io.c
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

pthread_t wifi_io_tid = (pthread_t)NULL;

/* =================================
 * void wifi_io_thread(void)
 */

void* wifi_io_thread(void *arg)
{
	printf("\tPowerMon: wifi_io_thread.\n");
	pthread_exit((void *)NULL);

	return (void *)NULL;
}

/* =================================
 * void wifi_io_thread_create(void)
 */

void wifi_io_thread_create(void)
{
    int err = pthread_create(
    		&wifi_io_tid,
    		NULL,
			&wifi_io_thread,
			NULL);

    if (err != 0)
        printf("\tUnable to create wifi_io_thread :[%s]\n", strerror(err));
}
