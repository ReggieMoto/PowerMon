/* =================================
 * wifi_io.c
 */

#include <stdio.h>
#include <unistd.h>
#include <stdio.h>

void wifi_io_thread(int *sig)
{
	pid_t pid = fork();

	if (pid == 0)
		printf("PowerMon: wifi_io_thread.\n");

	if (*sig == 0)
		*sig = 1;
}
