#include <stdio.h>

#include <libutil/endpoint.h>
#include <libutil/epoll.h>
#include <libutil/socket.h>
#include <libutil/thread.h>


void callback_test(int e, struct ep_event_view *v) {
    printf("event\n");
    ep_queue_push(v->queue, v->self);
}


void thread_test() {
    struct ep_thread_pool tp;
    struct ep_loop_data loop;
    ep_thread_pool_create(&tp, &loop.table, 4);
    ep_loop_init(&loop, &tp.queue);


    // create a socket acceptor
    struct ep_handler *h = ep_new_hdl(&loop.table, callback_test);
    ep_local_acceptor(&loop, h);


    // listen for incoming events
    ep_loop_run(&loop);
    ep_thread_pool_join(&tp);
}


int main(int argc, char *argv[]) {
    thread_test();
    return 0;
}
