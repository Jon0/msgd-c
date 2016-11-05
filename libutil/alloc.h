#ifndef ALLOC_H
#define ALLOC_H

#include <stdlib.h>
#include <stdint.h>


typedef int ep_alloc_id;
typedef size_t (*ep_cleanup_t)(void *);
typedef void (*ep_debug_t)(void *);


/*
 * allocate resources and create graph of dependency
 */
struct ep_alloc_container {

};


#endif
