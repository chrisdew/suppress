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
#include <sys/select.h>

#include "debug.h"
#include "suppress.h"
#include "state.h"


void loop(in_port_t port, in_addr_t group) {
	struct sockaddr_in addr, dest;
	socklen_t addrlen;
	int sock;
	int cnt;
	struct ip_mreq mreq;

	state_init();


	// set up socket
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		perror("socket");
		exit(1);
	}
	bzero((char *)&addr, sizeof(addr));
	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = group;
	dest.sin_port = htons(port);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	addrlen = sizeof(addr);

	// bind to listen for multicasts
	if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("bind");
		exit(1);
	}
	mreq.imr_multiaddr.s_addr = group;
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
			&mreq, sizeof(mreq)) < 0) {
		perror("setsockopt mreq");
		exit(1);
	}

	// loop for resetting select timer and sending packets
	while (1) {
		struct timeval tv = multicast_wait;
		struct sockaddr_in from_addr;
		from_addr.sin_addr.s_addr = group;

		// loop for receiving packets
		while (1) {
			fd_set readfds;
			FD_SET(sock, &readfds);
			int sret = select(sock + 1, &readfds, NULL, NULL, &tv);
			debug("sret: %i", sret);
			if (sret > 0) {
				char buf[4];
				size_t buflen = sizeof(buf);
				cnt = recvfrom(sock, buf, buflen, 0,
						(struct sockaddr *) &from_addr, &addrlen);
				uint32_t htonl_remote_pid = *((int *)buf);
				uint32_t remote_pid = ntohl(htonl_remote_pid);
				debug("remote_pid: %i", remote_pid);
				debug("remote_addr: %s", inet_ntoa(from_addr.sin_addr));
				debug("cnt: %i", cnt);
				state_push(from_addr.sin_addr, remote_pid);
				debug("continuing... %li, %li", tv.tv_sec, tv.tv_usec);
				continue;
			} else {
				state_poll();
				debug("sending...");
				uint32_t htonl_pid = htonl(getpid());
				cnt = sendto(sock, &htonl_pid, sizeof(uint32_t), 0,
						(struct sockaddr *) &dest, addrlen);
				debug("sent to: %s", inet_ntoa(dest.sin_addr));
				if (cnt != sizeof(uint32_t)) {
					perror("sendto");
					exit(1);
				}
				break;
			}
		}
	}
}

