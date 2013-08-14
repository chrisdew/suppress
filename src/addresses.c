#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#include "debug.h"

#define BUF_SZ 16



char *local_address(void) {
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    char iface[BUF_SZ] = "zzz";
    char *ipv4 = malloc(BUF_SZ);

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa ->ifa_addr->sa_family==AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            debug("%s IP Address %s", ifa->ifa_name, addressBuffer);
            if (strcmp(ifa->ifa_name, iface) < 0) {
            	debug("lower interface name");
            	strcpy(ipv4, addressBuffer);
            	strcpy(iface, ifa->ifa_name);
            }
        /* FIXME: IPv6 support
        } else if (ifa->ifa_addr->sa_family==AF_INET6) { // check it is IP6
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
        */
        }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
    return ipv4;
}
