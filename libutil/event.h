#ifndef LIBUTIL_EVENT_H
#define LIBUTIL_EVENT_H

#include "stdlib.h"


struct msgu_timer_event {

};


struct msgu_accept_event {

};


struct msgu_local_event {

};


struct msgu_network_event {

};


struct msgu_file_event {

};


struct msgu_callback_table {
    size_t event_id_size;
};


/*
 * lookup callback function using os event type
 */
void msgu_handle_event(const struct msgu_callback_table *table, void *os_event_id);


#endif
