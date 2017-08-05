/* =================================
 * main.c
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, int argv[])
{
	pid_t child_pid;

	/* Start of the program */
	printf("Starting up PowerMon.\n");

	/* Fork the process */
	child_pid = fork();

	if (child_pid != 0)
	{
	    printf ("This is the parent process, with id %d\n", (int) getpid ());
	    printf ("The child's process ID is %d\n",(int) child_pid );
	}
	else
	{
		printf ("This is the child process, with id %d\n", (int) getpid ());
	}

	return 0;
}
