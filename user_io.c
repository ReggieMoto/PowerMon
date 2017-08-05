/* =================================
 * user_io.c
 */

#include <stdio.h>
#include <unistd.h>
#include <stdio.h>

void user_io_thread(int *sig)
{
	pid_t pid = fork();

	if (pid == 0)
		printf("PowerMon: user_io_thread.\n");

	if (*sig == 0)
		*sig = 1;
}
