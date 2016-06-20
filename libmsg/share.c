#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>

#include "share.h"


void sharemem() {
    struct shmid_ds mem_stat;
    int mem_id = shmget(IPC_PRIVATE, 4096, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    char *mem_access =  (char *) shmat(mem_id, 0, 0);
    shmctl(mem_id, IPC_STAT, &mem_stat);
}
