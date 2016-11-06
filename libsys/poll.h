#ifndef POLL_H
#define POLL_H


typedef int (*ep_poll_callback_t)(void *, int id);


int ep_poll_create();


/*
 * wait for events and call a function
 */
int ep_poll_wait(int epfd, void *obj, ep_poll_callback_t cb);


/*
 * add a file descriptor to epoll
 */
void ep_poll_enable(int epfd, int epid, int fd);


#endif
