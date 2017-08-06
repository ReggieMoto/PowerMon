/* =================================
 * main.c
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

extern void* user_io_thread_create(void);
extern void* wifi_io_thread_create(void);
extern void* data_store_thread_create(void);

extern pthread_t wifi_io_tid;
extern pthread_t user_io_tid;
extern pthread_t data_store_tid;

int main(int argc, int argv[])
{
    printf ("Spawning the user_io_thread.\n");
    user_io_thread_create();

    printf ("Spawning the wifi_io_thread.\n");
   	wifi_io_thread_create();

    printf ("Spawning the data_store_thread.\n");
   	data_store_thread_create();

    pthread_join(wifi_io_tid, (void**)NULL);
    pthread_join(wifi_io_tid, (void**)NULL);
    pthread_join(wifi_io_tid, (void**)NULL);

    printf("All threads terminated.\n");

	return 0;
}
