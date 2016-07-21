#include <stdio.h>

#include <libutil/endpoint.h>
#include <libutil/socket.h>
#include <libutil/thread.h>


void callback_test(int e, struct ep_event_view *v) {
    printf("event\n");
    ep_queue_push(v->queue, v->self);
}


void thread_test() {
    struct ep_thread_pool tp;
    struct ep_table table;
    ep_table_init(&table);
    ep_thread_pool_create(&tp, &table, 4);


    // create a socket acceptor
    struct ep_handler *h = ep_new_hdl(&table, callback_test);
    ep_local_acceptor(&table, h);


    // listen for incoming events
    struct ep_event ev;
    struct ep_source *src [32];
    while (1) {
        int r = ep_table_wait(&table, src, 32);
        for (int i = 0; i < r; ++i) {
            printf("recv event\n");
            ev.hdl = ep_table_hdl(&table, src[i]->epid);
            ev.recv = src[i]->func;
        }
    }


    ep_thread_pool_join(&tp);
    ep_table_free(&table);
}


int main(int argc, char *argv[]) {
    thread_test();
    return 0;
}
