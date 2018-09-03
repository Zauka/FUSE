#ifndef __client_syscalls__
#define __client_syscalls__

#include "../utils/macros.h"

#define FUSE_DATA ((struct fuse_info*) fuse_get_context()->private_data)
#define LOG(...) fprintf(__VA_ARGS__)
// To log in config-provided log file
// LOG(FUSE_DATA->log, "message you wanna log with variables %s/%d...", variables)


struct fuse_info{
  FILE* log;
  char logfile[MAXLEN];
  char start_dir[MAXLEN];

  int sfd; // for socket sending
};

void* client_init(struct fuse_conn_info *info);

int client_readlink(const char *path, char *link, size_t size);
int client_getattr(const char *path, struct stat *statbuf);

int client_open(const char *path, struct fuse_file_info *fi);
int client_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
int client_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);

int client_mkdir(const char *path, mode_t mode);
int client_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
int client_rmdir(const char *path);
int client_opendir(const char *path, struct fuse_file_info *fi);
int client_unlink(const char *path);
int client_release(const char *path, struct fuse_file_info *fi);
int client_releasedir(const char *path, struct fuse_file_info *fi);
int client_rename(const char *path, const char *newpath);

void client_destroy(void *userdata);

// real_path should have enough space already allocated to
// write down corresponding string
void gen_real_path(char* real_path, const char* fuse_path);

#endif //__client_syscalls__

//
// TODO: create

/*
// no .getdir -- that's deprecated
.getdir = NULL,
.mknod = bb_mknod,
.mkdir = bb_mkdir,
.unlink = bb_unlink,
.rmdir = bb_rmdir,
.symlink = bb_symlink,
.rename = bb_rename,
.link = bb_link,
.chmod = bb_chmod,
.chown = bb_chown,
.truncate = bb_truncate,
.utime = bb_utime,

.statfs = bb_statfs,
.flush = bb_flush,
.release = bb_release,
.fsync = bb_fsync,

#ifdef HAVE_SYS_XATTR_H
.setxattr = bb_setxattr,
.getxattr = bb_getxattr,
.listxattr = bb_listxattr,
.removexattr = bb_removexattr,
#endif

.opendir = bb_opendir,
.readdir = bb_readdir,
.releasedir = bb_releasedir,
.fsyncdir = bb_fsyncdir,
.init = bb_init,
.destroy = bb_destroy,
.access = bb_access,
.ftruncate = bb_ftruncate,
.fgetattr = bb_fgetattr
*/
