#ifndef __server_syscalls__
#define __server_syscalls__

#include "../utils/macros.h"

int server_readlink(const char *path, char *link, size_t size);
int server_getattr(const char *path, struct stat *statbuf);

int server_open(const char *path, int flags);
int server_read(const char *path, char *buf, size_t size, off_t offset);
int server_write(const char *path, const char *buf, size_t size, off_t offset);

int server_mkdir(const char *path, mode_t mode);
int server_readdir(const char *path, void *buf, off_t offset);
int server_rmdir(const char *path);
int server_opendir(const char *path);
int server_unlink(const char *path);
int server_release(const char *path);
int server_releasedir(const char *path);
int server_rename(const char *path, const char *newpath);

void server_destroy(void *userdata);



#endif //__server_syscalls__
