#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#include "debug.h"
#include "suppress.h"
#include "addresses.h"

#define SUPPRESSED 1
#define UNSUPPRESSED 2

static char *state_names[] = {"INVALID", "SUPPRESSED", "UNSUPPRESSED"};

static int state = SUPPRESSED;

static int64_t last_suppressant_ms;
static char last_suppressant_ipv4[16];
static uint32_t last_suppressant_pid;

static struct in_addr local_ipv4;
static uint32_t local_pid;

static pid_t child_pid;

static int64_t now_ms() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return ((int64_t) tv.tv_sec) * 1000 + ((int64_t) tv.tv_usec) / 1000;
}

sig_atomic_t child_exit_status;

void clean_up_child_process (int signal_number) {
	debug("SIGCHILD received");
	/* Clean up the child process. */
	int status;
	wait (&status);
	/* Store its exit status in a global variable. */
	child_exit_status = status;
	debug("child_exit_status %i", status);
}


int child_init(const char *command, char *const args[], char *const env[]) {
	debug("child_init1");

	int ret = 0;

	struct sigaction sigchld_action;
	memset (&sigchld_action, 0, sizeof (sigchld_action));
	sigchld_action.sa_handler = &clean_up_child_process;
	sigaction (SIGCHLD, &sigchld_action, NULL);

	int pid = fork();
	if (pid == 0) {
		log_prefix = "|CHILD|  ";
		int ret = 0;
		debug("child start, command: %p, args: %p, env: %p", command, args, env);
		/* Handle SIGCHLD by calling clean_up_child_process. */

		ret = execve(command, args, env);

		// if we get here at all, an error occurred, but we are in the child
		// process, so just exit
		err:
		log_err("exec of the child process %i", ret);
		exit(ret);
	} else if (pid > 0) {
		child_pid = pid;
		return 0;
	} else {
		log_err("fork failed");
	}
	return -1;
}

static void state_change(int new_state) {
	debug("state changing from %s to %s", state_names[state], state_names[new_state]);

	if (new_state == UNSUPPRESSED && state == SUPPRESSED) {
		debug("spawning");
		char *const dummy[] = { NULL };
		child_init(cmd.argv[0], cmd.argv, dummy);
	} else if (new_state == SUPPRESSED && state == UNSUPPRESSED) {
		debug("sending SIGTERM");
		kill(child_pid, SIGTERM);
		sleep(1);
		sleep(1);
		debug("sending SIGKILL");
		kill(child_pid, SIGKILL);
	}
	state = new_state;
}

void state_init(void) {
	last_suppressant_ms = now_ms();
	strncpy(last_suppressant_ipv4, "none", 5);
	last_suppressant_pid = 0;

	local_pid = getpid();
	char *ipv4 = local_address();
	inet_aton(ipv4, &local_ipv4);
}

void suppress(char *ipv4, uint32_t pid) {
    debug("suppress(%s,%i)", ipv4, pid);
	last_suppressant_ms = now_ms();
	strncpy(last_suppressant_ipv4, ipv4, 16);
	last_suppressant_ipv4[15] = '\0'; // just make sure
	last_suppressant_pid = pid;

	switch (state) {
	case SUPPRESSED:
		break;
	case UNSUPPRESSED:
		state_change(SUPPRESSED);
		break;
	}
}

void state_push(struct in_addr addr, uint32_t pid) {
	char *ipv4 = inet_ntoa(addr);
	if (strncmp(ipv4, "127.0.0.1", 16) == 0) {
		if (pid < local_pid) {
			suppress(ipv4, pid);
		}
	} else {
		debug("addr: %s", inet_ntoa(addr));
		debug("local_ipv4: %s", inet_ntoa(local_ipv4));
		int cmp = memcmp(&addr, &local_ipv4, sizeof(struct in_addr));
		if (cmp < 0) {
			suppress(ipv4, pid);
		} else if (cmp == 0) {
			if (pid < local_pid) {
				suppress(ipv4, pid);
			}
		}
	}
}

void unsuppress(void) {
	debug("unsuppress()");
	switch (state) {
	case SUPPRESSED:
		state_change(UNSUPPRESSED);
		break;
	case UNSUPPRESSED:
		break;
	}
}

void state_poll() {
	if (now_ms() > last_suppressant_ms + wait_ms) {
		unsuppress();
	}
}
