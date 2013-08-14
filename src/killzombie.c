#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

pid_t child_pid;

sig_atomic_t child_exit_status;

void clean_up_child_process (int signal_number) {
	printf("signal received\n");
	/* Clean up the child process. */
	int status;
	wait (&status);
	/* Store its exit status in a global variable. */
	child_exit_status = status;
	printf("child_exit_status %i\n", status);
}

int main(int argc, char **argv) {
	printf("main\n");

	int pid = fork();

	signal(SIGCHLD, clean_up_child_process);
	if (pid == 0) {
		printf("child process\n");

		char *args[] = { "/bin/sleep", "3", NULL };
		char *env[] = { NULL };
		printf("child start, command: %s, args: %p, env: %p\n", args[0], args, env);
		int ret = execve(args[0], args, env);

		// if we get here, then the execve has failed
		printf("exec of the child process failed %i\n", ret);
	} else if (pid > 0) {
		printf("parent process\n");
		child_pid = pid;
	} else {
		perror("fork failed\n");
	}
	printf("parent sleeping for 10s\n");
	sleep(10);
	return 0;
}
