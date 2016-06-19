#ifndef MSGD_H
#define MSGD_H

extern void md_register_proc(const char *name, int mode);
extern void md_register_pipe(const char *type, const char *name, int mode);

#endif
