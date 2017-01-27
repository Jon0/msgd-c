#ifndef LIBUTIL_FILEPATH_H
#define LIBUTIL_FILEPATH_H

#include <libutil/stream.h>

#include "node.h"
#include "string.h"


/*
 * a file or process
 */
struct msgu_resource {
    struct msgu_node node;
    struct msgu_stream stream;
};



struct msgu_file_params {

};


typedef int (*msgu_path_stat_t)(struct msgu_node *, const char *, struct msgu_file_params *);
typedef int (*msgu_path_open_t)(struct msgu_resource *, const char *, struct msgu_file_params *);


struct msgu_file_ops {
    msgu_path_stat_t stat;
    msgu_path_open_t open;
};


/*
 * find the file name component of a path
 */
int msgu_filename(struct msgu_string *dest, const char *full_path);


#endif
