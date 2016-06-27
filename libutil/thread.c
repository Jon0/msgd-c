#include <stdio.h>

#include "thread.h"


void *print_message_function(void *msg) {
    printf("%s\n", (const char *) msg);
    return NULL;
}


void thread_init(struct thread_pool *tp, size_t thread_count) {
    tp->tmem = malloc(sizeof(pthread_t) * thread_count);
    const char *msg = "ThreadMsg";
    for (int i = 0; i < thread_count; ++i) {
        int err = pthread_create(&tp->tmem[i], NULL, print_message_function, (void *) msg);
        if (err) {
            perror("pthread_create");
            return;
        }
    }
    for (int i = 0; i < thread_count; ++i) {
        pthread_join(tp->tmem[i], NULL);
    }
}
