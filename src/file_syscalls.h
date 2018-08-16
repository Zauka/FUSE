#ifndef __file_syscalls__
#define __file_syscalls__

int zfs_readlink(const char *path, char *link, size_t size);
int zfs_getattr(const char *path, struct stat *statbuf);

int zfs_open(const char *path, struct fuse_file_info *fi);
int zfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
int zfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);

int zfs_mkdir(const char *path, mode_t mode);
int zfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
int zfs_rmdir(const char *path);
int zfs_opendir(const char *path, struct fuse_file_info *fi);
int zfs_unlink(const char *path);
int zfs_release(const char *path, struct fuse_file_info *fi);
int zfs_releasedir(const char *path, struct fuse_file_info *fi);
int zfs_rename(const char *path, const char *newpath);

#endif

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
