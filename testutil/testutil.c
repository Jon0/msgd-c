#include <stdio.h>

#include <libutil/endpoint.h>
#include <libutil/handler.h>
#include <libutil/socket.h>
#include <libutil/thread.h>


void callback_test(int e, struct ep_event_view *v) {
    printf("event\n");
    ep_queue_push(v->queue, v->self);
}


void thread_test() {
    struct ep_thread_pool tp;
    struct ep_hdlset s;
    ep_hdlset_init(&s, 256);
    ep_thread_pool_create(&tp, 4);

    // add a test event
    struct ep_event ev;
    ev.hdl = ep_handler_create(&s, callback_test);
    ep_queue_push(&tp.queue, &ev);

    ep_thread_pool_join(&tp);
}


int main(int argc, char *argv[]) {
    thread_test();
    return 0;
}
