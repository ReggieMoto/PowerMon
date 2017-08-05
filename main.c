/* =================================
 * main.c
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

extern void user_io_thread(int *sig);
extern void wifi_io_thread(int *sig);

int main(int argc, int argv[])
{
	int sig=0;

    printf ("This is the parent process, with id %d\n", (int) getpid ());

    user_io_thread(&sig);

    if (sig)
    	wifi_io_thread(&sig);

	return 0;
}
