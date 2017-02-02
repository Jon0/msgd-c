#ifndef LIBUTIL_DATATABLE_H
#define LIBUTIL_DATATABLE_H

#include "serial.h"


struct msgu_datatable {
    char   *data;
    size_t *sizes;
    size_t  sizes_count;
    size_t  rowsize;
    size_t  row_count;
    size_t  allocated;
};


struct msgu_datamap {
    struct msgu_datatable *table;
    size_t item;
};


void msgu_datatable_init(struct msgu_datatable *t, size_t *sizes, size_t count);
void msgu_datatable_alloc(struct msgu_datatable *t, size_t max);


/*
 * creates space for new rows
 */
int msgu_datatable_emplace(struct msgu_datatable *t, void **table, size_t count);


/*
 * prepare keys after item is emplaced
 */
int msgu_datatable_create_key(struct msgu_datamap *m, int index);


#endif
