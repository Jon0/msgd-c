#ifndef HANDLER_H
#define HANDLER_H

typedef void (*event_t)(int exception);


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
 * a condition to wait for, and functions
 */
void ep_hdl_wait(event_t ev);


#endif
