#ifndef STATE_H
#define STATE_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void state_init(void);
void state_poll(void);
void state_push(struct in_addr addr, uint32_t pid);

#endif
