#ifndef __server_syscalls__
#define __server_syscalls__

#include "../utils/macros.h"

void* server_init(struct fuse_conn_info *info);

int server_readlink(const char *path, char *link, size_t size);
int server_getattr(const char *path, struct stat *statbuf);

int server_open(const char *path, struct fuse_file_info *fi);
int server_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
int server_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);

int server_mkdir(const char *path, mode_t mode);
int server_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
int server_rmdir(const char *path);
int server_opendir(const char *path, struct fuse_file_info *fi);
int server_unlink(const char *path);
int server_release(const char *path, struct fuse_file_info *fi);
int server_releasedir(const char *path, struct fuse_file_info *fi);
int server_rename(const char *path, const char *newpath);

void server_destroy(void *userdata);

// real_path should have enough space already allocated to
// write down corresponding string
void gen_real_path(char* real_path, const char* fuse_path);

#endif //__server_syscalls__
