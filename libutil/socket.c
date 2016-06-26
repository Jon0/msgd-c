#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>

#include "socket.h"

void sk_open(char *address) {
    int sd;
    struct sockaddr_un saun, fsaun;
    if ((sd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
    }


    saun.sun_family = AF_UNIX;
    strcpy(saun.sun_path, address);
    unlink(address);
    int len = sizeof(saun.sun_family) + strlen(saun.sun_path);

    if (bind(sd, &saun, len) < 0) {
        perror("server: bind");
        exit(1);
    }
}
