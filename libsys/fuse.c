#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <fuse.h>
#include <fuse/fuse_common.h>
#include <fuse/fuse_lowlevel.h>

#include "fuse.h"


void *msgs_fuse_thread(void *p) {
	struct msgs_fuse_files *f = p;
	printf("starting fuse loop\n");
	if (fuse_loop(f->fuse) < 0) {
		perror("fuse_loop");
	}
}


int msgs_fuse_getattr(const char *path, struct stat *stbuf) {
		printf("fuse getattr: %s\n", path);
        int res = 0;
        memset(stbuf, 0, sizeof(struct stat));
        if (strcmp(path, "/") == 0) {
                stbuf->st_mode = S_IFDIR | 0755;
                stbuf->st_nlink = 2;
        }
		else if (strcmp(path, "/test") == 0) {
                stbuf->st_mode = S_IFREG | 0444;
                stbuf->st_nlink = 1;
                stbuf->st_size = strlen("test");
        }
		else {
            res = -ENOENT;
		}
		return res;
   }



int msgs_fuse_access(const char *path, int mask) {
	printf("fuse access: %s\n", path);
    return 0;
}


int msgs_fuse_opendir(const char *path, struct fuse_file_info *fileInfo) {
    printf("fuse opendir: %s\n", path);
    return 0;
}


int msgs_fuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi) {
	printf("fuse readdir: %s\n", path);


	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, "test", NULL, 0);

	return 0;
}


int msgs_fuse_open(const char* path, struct fuse_file_info* fi) {
	printf("fuse open: %s\n", path);
	return 0;
}


int msgs_fuse_read(const char* path, char *buf, size_t size, off_t offset, struct fuse_file_info* fi) {
	printf("fuse read: %s, %lu\n", path, size);
	const char *teststr = "12345\n";
	size_t count = strlen(teststr);
	if (strcmp(path, "/test") == 0) {
		memcpy(buf, teststr, count);
		return count;
	}
	return 0;
}


int msgs_fuse_flush(const char *path, struct fuse_file_info *fileInfo) {
	printf("fuse flush: %s\n", path);
    return 0;
}

int msgs_fuse_release(const char *path, struct fuse_file_info *fileInfo) {
	printf("fuse release: %s\n", path);
    return 0;
}

int msgs_fuse_fsync(const char *path, int datasync, struct fuse_file_info *fi) {
	printf("fuse fsync: %s\n", path);
    return 0;
}


static struct fuse_operations msgs_fuse_ops = {
	.getattr     = msgs_fuse_getattr,
	.access      = msgs_fuse_access,
	.opendir     = msgs_fuse_opendir,
    .readdir     = msgs_fuse_readdir,
    .open        = msgs_fuse_open,
    .read        = msgs_fuse_read,
	.flush       = msgs_fuse_flush,
	.release     = msgs_fuse_release,
	.fsync       = msgs_fuse_fsync,
};


void msgs_fuse_set_dir(struct msgs_fuse_files *f, const char *subdir) {
	char cwd [1024];
	if (getcwd(cwd, sizeof(cwd))) {
		sprintf(f->mountpoint, "%s/%s", cwd, subdir);
		printf("fuse mountpoint: %s\n", f->mountpoint);
	}
}


int msgs_fuse_init(struct msgs_fuse_files *f) {
	char *argv[1];
    argv[0] = "msgd";
	struct fuse_args args = FUSE_ARGS_INIT(1, argv);

	// init state
	f->ch = NULL;
	f->fuse = NULL;

	fuse_unmount(f->mountpoint, f->ch);
	f->ch = fuse_mount(f->mountpoint, &args);
	f->fuse = fuse_new(f->ch, &args, &msgs_fuse_ops, sizeof(msgs_fuse_ops), f);
	fuse_opt_free_args(&args);
    return 1;
}


void msgs_fuse_loop(struct msgs_fuse_files *f) {
	int err = pthread_create(&f->fuse_thread, NULL, msgs_fuse_thread, f);
	if (err) {
		perror("pthread_create");
	}
}


void msgs_fuse_free(struct msgs_fuse_files *f) {
	pthread_join(f->fuse_thread, NULL);
	fuse_unmount(f->mountpoint, f->ch);
}
