#include <string.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <unistd.h>

#include "network.h"


void msgs_host_init_self(struct msgu_host *h) {
    struct ifaddrs *addr, *ifa;
    int family, s, n;
    char host [32];

    getifaddrs(&addr);
    for (ifa = addr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
        if (ifa->ifa_addr == NULL) continue;
        family = ifa->ifa_addr->sa_family;
        if (family == AF_INET || family == AF_INET6) {
            s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            if (s == 0) {
                strcpy(h->addr.data, host);
            }
        }
    }
    freeifaddrs(addr);

    // get the string name of host
    gethostname(h->hostname, sizeof(h->hostname));
}
