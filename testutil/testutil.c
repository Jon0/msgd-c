#include <stdio.h>

#include <libutil/buffer.h>
#include <libutil/endpoint.h>
#include <libutil/socket.h>
#include <libutil/thread.h>


void thread_main(struct ep_thread_view *v) {
    // test thread
    printf("thread\n");
}


void thread_test() {
    struct ep_thread_pool tp;
    ep_thread_pool_init(&tp);
    ep_thread_pool_start(&tp, thread_main);
    ep_thread_pool_join(&tp);
    ep_thread_pool_free(&tp);
}


int main(int argc, char *argv[]) {
    thread_test();
    return 0;
}
