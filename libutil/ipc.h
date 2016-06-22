#ifndef IPC_H
#define IPC_H

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define QUEUE_NAME "/local_msgd"


struct ipc_channel {
    char queue_name [256];
    mqd_t queue_id;
};


void mdu_ipc_server(struct ipc_channel *c, const char *name);
void mdu_ipc_client(struct ipc_channel *c, const char *name);
void mdu_ipc_close(struct ipc_channel *c);

ssize_t mdu_ipc_send(struct ipc_channel *c, char *buf, size_t count);
ssize_t mdu_ipc_recv(struct ipc_channel *c, char *buf, size_t count);


#endif
