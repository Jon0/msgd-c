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


void msgu_datatable_init(struct msgu_datatable *t, size_t *sizes, size_t count);
void msgu_datatable_alloc(struct msgu_datatable *t, size_t max);


/*
 * returns pointers of each row element
 */
int msgu_datatable_get(struct msgu_datatable *t, void **table, size_t index);


/*
 * creates space for new rows
 */
int msgu_datatable_emplace(struct msgu_datatable *t, void **table);


/*
 * creates a hash map
 */
struct msgu_datamap {
    struct msgu_datatable *table;
    size_t                 item;
    msgu_map_hash_t        hash;
    msgu_map_cmp_t         cmp;
    size_t                 alloc;
    size_t                 chain;
    char                  *data;
};


void msgu_datamap_init(struct msgu_datamap *m, struct msgu_datatable *t, size_t item, msgu_map_hash_t hash, msgu_map_cmp_t cmp);
void msgu_datamap_alloc(struct msgu_datamap *m, size_t max);


/*
 * prepare keys after item is emplaced
 */
int msgu_datamap_create_key(struct msgu_datamap *m, int index);


#endif
