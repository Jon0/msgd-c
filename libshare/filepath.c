#include "filepath.h"


int msgu_filename(struct msgu_string *dest, const char *full_path) {
    char *dup = strdup(full_path);
    char *string = dup;
    char *token;
    char *nonempty = NULL;
    while ((token = strsep(&string, "/")) != NULL) {
        if (strlen(token) > 0) {
            nonempty = token;
        }
    }
    if (nonempty) {
        msgu_string_from_static(dest, nonempty);
    }
    free(dup);
    return (nonempty != NULL);
}
