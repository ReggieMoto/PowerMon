/* =================================
 * user_io.c
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

pthread_t user_io_tid = (pthread_t)NULL;

/* =================================
 * void user_io_thread(void)
 */

void* user_io_thread(void *arg)
{
	printf("\tPowerMon: user_io_thread.\n");
	pthread_exit((void *)NULL);

	return (void *)NULL;
}

/* =================================
 * void user_io_thread_create(void)
 */
void user_io_thread_create(void)
{
    int err = pthread_create(
    		&user_io_tid,
    		NULL,
			&user_io_thread,
			NULL);

    if (err != 0)
        printf("\tUnable to create user_io_thread :[%s]\n", strerror(err));
}
