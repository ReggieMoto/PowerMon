/* =================================
 * main.c
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, int argv[])
{
	pid_t user_io_pid;
	pid_t wifi_io_pid;

	/* Start of the program */
	printf("Starting up PowerMon.\n");

	/* Fork the process */
	user_io_pid = fork();
	wifi_io_pid = fork();

	if ((user_io_pid != 0) && (wifi_io_pid != 0))
	{
	    printf ("This is the parent process, with id %d\n", (int) getpid ());
	}
	else
	{
		if (getpid() == user_io_pid)
		    printf ("The User I/O process ID is %d\n",(int) user_io_pid );
		else
		    printf ("The Wifi I/O process ID is %d\n",(int) wifi_io_pid );
	}

	return 0;
}
