#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "fuse.h"


int ep_fuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi) {

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, "test", NULL, 0);

	return 0;
}


int ep_fuse_open(const char* path, struct fuse_file_info* fi) {
	return 0;
}


int ep_fuse_read(const char* path, char *buf, size_t size, off_t offset, struct fuse_file_info* fi) {
	return 0;
}


static struct fuse_operations ep_fuse_ops = {
    .readdir     = ep_fuse_readdir,
    .open        = ep_fuse_open,
    .read        = ep_fuse_read,
};


int ep_fuse_init(struct ep_fuse_config *cfg) {
    char *argv[1];
    argv[0] = "name";
    // fuse_mount
    return fuse_main(1, argv, &ep_fuse_ops, cfg);
}
