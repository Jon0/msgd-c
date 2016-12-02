#ifndef LIBSYS_MEMORY_H
#define LIBSYS_MEMORY_H

#include <unistd.h>
#include <sys/syscall.h>


static inline int memfd_create(const char *name, unsigned int flags) {
    return syscall(__NR_memfd_create, name, flags);
}


void *ep_memfile(const char *filepath, size_t count);


#endif
