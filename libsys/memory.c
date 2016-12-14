#include <stdio.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "memory.h"


void *msgs_memfile(const char *filepath, size_t count) {

    // TODO: return fd to close
    int fd = memfd_create(filepath, 0);
    if (fd < 0) {
        perror("open");
        return NULL;
    }
    int al = ftruncate(fd, count);
    if (al == -1) {
        perror("ftruncate");
        return NULL;
    }
    void *ptr = mmap(NULL, count, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == (void *) -1) {
        perror("mmap");
        return NULL;
    }
    return ptr;
}


void sharemem() {
    struct shmid_ds mem_stat;
    int mem_id = shmget(IPC_PRIVATE, 4096, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    char *mem_access =  (char *) shmat(mem_id, 0, 0);
    shmctl(mem_id, IPC_STAT, &mem_stat);
}
