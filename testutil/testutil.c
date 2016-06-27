#include <stdio.h>
#include <unistd.h>

#include <libutil/thread.h>


void *printtest(void *p) {
    struct thread_task t;
    t.fn = printtest;
    struct thread_queue *q = (struct thread_queue *) p;
    printf("thread test\n");
    queue_ins(q, &t);
    sleep(1);
}


void threadtest() {
    struct thread_pool p;
    struct thread_task t;
    pool_init(&p, 8);

    t.fn = printtest;
    queue_ins(&p.queue, &t);
    sleep(10);

    queue_stop(&p.queue);
    pool_join(&p);
    printf("threads joined\n");
}


int main(int argc, char *argv[]) {
    threadtest();
}
