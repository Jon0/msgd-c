#ifndef HANDLER_H
#define HANDLER_H

#include <sys/epoll.h>


/*
 * condition for running the function
 */
struct ep_hdl_cond {

};


/*
 * one function at a time, per buffer
 */
struct ep_buffer_hdl {

};


/*
 * a condition to wait for, and a function
 */
void ep_hdl_wait();


#endif
