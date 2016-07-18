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
    struct ep_hdlset hdls;
    struct ep_thread_pool tp;
    struct ep_loop_data loop;
    ep_hdlset_init(&hdls, 256);
    ep_thread_pool_create(&tp, 4);
    ep_loop_init(&loop, &tp.queue);

    // add a test event
    //struct ep_event ev;
    //ev.hdl = ep_handler_create(&s, callback_test);
    //ep_queue_push(&tp.queue, &ev);

    // create a socket acceptor
    const char *testpath = "utiltest";
    struct ep_address *addr = ep_new_addr(&loop.table);
    ep_unlink(testpath);
    ep_set_local(addr, testpath);
    ep_add_pipe_endpoints(&loop.table, addr->epid);
    ep_activate_acceptor(addr);

    // listen for incoming events
    struct ep_handler *h = ep_handler_create(&hdls, callback_test);
    ep_loop_source(&loop, h);
    ep_loop_run(&loop);

    ep_thread_pool_join(&tp);
}


int main(int argc, char *argv[]) {
    thread_test();
    return 0;
}
