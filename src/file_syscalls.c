#include <fuse.h>
#include <stdio.h>
#include "file_syscalls.h"
#include "constants.h"

int zfs_readlink(const char *path, char *link, size_t size)
{
  printf("invoking readlink\n");
  return SUCC;
}

int zfs_getattr(const char *path, struct stat *statbuf)
{
    printf("invoking getattr\n");
    return SUCC;
}

int zfs_open(const char *path, struct fuse_file_info *fi)
{
    printf("invoking open\n");
    return SUCC;
}

int zfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    printf("invoking read\n");
    return SUCC;
}

int zfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    printf("invoking write\n");
    return SUCC;
}

int zfs_mkdir(const char *path, mode_t mode)
{
    printf("invoking mkdir\n");
    return SUCC;
}

int zfs_rmdir(const char *path)
{
    printf("invoking rmdir\n");
    return SUCC;
}

int zfs_opendir(const char *path, struct fuse_file_info *fi)
{
    printf("invoking opendir\n");
    return SUCC;
}

int zfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
  printf("invoking readdir\n");
  return SUCC;
}

int zfs_unlink(const char *path)
{
  printf("invoking unlink\n");
  return SUCC;
}

int zfs_release(const char *path, struct fuse_file_info *fi)
{
    printf("invoking release\n");
    return SUCC;
}

int zfs_releasedir(const char *path, struct fuse_file_info *fi)
{
    printf("invoking releasedir\n");
    return SUCC;
}

int zfs_rename(const char *path, const char *newpath)
{
    printf("invoking rename\n");
    return SUCC;
}
