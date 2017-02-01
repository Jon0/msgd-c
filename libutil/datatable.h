#ifndef LIBUTIL_DATATABLE_H
#define LIBUTIL_DATATABLE_H

#include "serial.h"


struct msgu_datatable {
    const struct msgu_element **fns;
    char   *data;
    size_t *sizes;
    size_t  count;
    size_t  rowsize;
    size_t  allocated;
};


void msgu_datatable_init(struct msgu_datatable *t, size_t *sizes, size_t count);
void msgu_datatable_alloc(struct msgu_datatable *t, size_t max);


/*
 * creates space for new rows
 */
int msgu_datatable_emplace(struct msgu_datatable *t, void **table, size_t count);


#endif
