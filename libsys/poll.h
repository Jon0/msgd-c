#ifndef POLL_H
#define POLL_H


int ep_poll_create();


/*
 * wait for events and return the sources
 */
int ep_poll_wait(int epfd, int *src, size_t count);


/*
 * add a file descriptor to epoll
 */
void ep_poll_enable(int epfd, int epid, int fd);


#endif
