#include <stdio.h>

#include <libutil/buffer.h>
#include <libutil/endpoint.h>
#include <libutil/socket.h>
#include <libutil/thread.h>


void thread_test() {
    struct ep_thread_pool tp;
    ep_thread_pool_create(&tp, 4);
    ep_thread_pool_join(&tp);
}


int main(int argc, char *argv[]) {
    thread_test();
    return 0;
}
