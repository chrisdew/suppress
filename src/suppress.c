/*
 ============================================================================
 Name        : suppress.c
 Author      : Chris Dew
 Version     :
 Copyright   : (C) Thorcom Systems Ltd. 2013, All Rights Reserved
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "debug.h"
#include "commander.h"
#include "addresses.h"
#include "loop.h"

//struct timeval wait = {.tv_sec=3, .tv_usec=100000};
int64_t wait_ms = 3000;
struct timeval multicast_wait = {.tv_sec=1};
command_t cmd;

static void wait_handler(command_t *self) {
	printf("wait: %sms before spawning", self->arg);
	// FIXME: check for errors
	int tmp = atoi(self->arg);
    int64_t wait_ms = tmp;
	//wait.tv_sec = tmp / 1000;
	//wait.tv_usec = (tmp % 1000) * 1000;
}

static void multicast_wait_handler(command_t *self) {
	printf("wait: %sms between multicasts", self->arg);
	// FIXME: check for errors
	int tmp = atoi(self->arg);
	multicast_wait.tv_sec = tmp / 1000;
	multicast_wait.tv_usec = (tmp % 1000) * 1000;
}

int main(int argc, const char **argv) {
	puts("Hello World"); /* prints Hello World */

	command_init(&cmd, argv[0], "0.0.1", "[options] command");
	command_option(&cmd, "-w", "--wait <ms>", "wait before spawning, default 3100ms", wait_handler);
	command_option(&cmd, "-m", "--multicast-wait <ms>", "wait between multicasts, default 1000ms", multicast_wait_handler);
	command_parse(&cmd, argc, argv);
	printf("additional args:\n");
	for (int i = 0; i < cmd.argc; ++i) {
	  printf("  - '%s'\n", cmd.argv[i]);
	}

	loop(2345, inet_addr("239.0.0.0"));
	command_free(&cmd);


	return EXIT_SUCCESS;
}
