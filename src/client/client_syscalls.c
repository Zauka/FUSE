#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>

#include <fuse.h>

#include "client_syscalls.h"
#include "../utils/macros.h"

/*
*   prototypes of this functions were taken from https://www.cs.nmsu.edu/~pfeiffer/fuse-tutorial/
*
*   Therefore this content belongs to:
*   Joseph J. Pfeiffer, Jr., Ph.D.
*   Emeritus Professor
*   Department of Computer Science
*   New Mexico State University
*   pfeiffer@cs.nmsu.edu
*/


void* client_init(struct fuse_conn_info *info)
{
  FUSE_DATA->log = fopen (FUSE_DATA->logfile, "w");

  /*
  *   this client implementation taken from provided socket connection example
  *   from OS_2018_Spring, link : https://classroom.google.com/u/0/c/MTIxNTI5NDEyODZa
  */
  int sfd;
  struct sockaddr_in addr;
  int ip;
  // char buf[1024];
  sfd = socket(AF_INET, SOCK_STREAM, 0);
  inet_pton(AF_INET, "127.0.0.1", &ip);

  addr.sin_family = AF_INET;
  addr.sin_port = htons(5000);
  addr.sin_addr.s_addr = ip;

  connect(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
  // TESTING::
  // char* msg = strdup ("are\nyou\nkidding\nme\n?\n\n\n\n\n\n\n\n\n\n\n\n\n");
  // write(sfd, msg, strlen(buffer)+1);
  // read(sfd, &buf, 1024);
  // printf ("client recieved %s\n", buf);
  FUSE_DATA->sfd = sfd;

  LOG (FUSE_DATA->log, "Logfile is %s\n", FUSE_DATA->logfile);
  return FUSE_DATA;
}

int client_readlink(const char *path, char *link, size_t size)
{
  printf("invoking readlink\n");
  return SUCC;
}

void add_token (char* str, const char* token)
{
  strcat(str, token);
  int len = strlen (str);
  str[len] = C_DELIM;
  str[len +1] = '\0';
}

void buf_to_stat (char* buf, struct stat *statbuf)
{
  statbuf->st_mode  = *(mode_t*)  buf;        buf += sizeof(mode_t);
  statbuf->st_ino   = *(ino_t*)   buf;        buf += sizeof(ino_t);
  statbuf->st_dev   = *(dev_t*)   buf;        buf += sizeof(dev_t);
  statbuf->st_rdev  = *(dev_t*)   buf;        buf += sizeof(dev_t);
  statbuf->st_nlink = *(nlink_t*) buf;        buf += sizeof(nlink_t);
  statbuf->st_uid   = *(uid_t*)   buf;        buf += sizeof(uid_t);
  statbuf->st_gid   = *(gid_t*)   buf;        buf += sizeof(gid_t);
  statbuf->st_size  = *(off_t*)   buf;        buf += sizeof(off_t);

  statbuf->st_atim.tv_sec = *(time_t*) buf;   buf += sizeof(time_t);
  statbuf->st_atim.tv_nsec = *(long*)  buf;   buf += sizeof(long);
  statbuf->st_mtim.tv_sec = *(time_t*) buf;   buf += sizeof(time_t);
  statbuf->st_mtim.tv_nsec = *(long*)  buf;   buf += sizeof(long);
  statbuf->st_ctim.tv_sec = *(time_t*) buf;   buf += sizeof(time_t);
  statbuf->st_ctim.tv_nsec = *(long*)  buf;   buf += sizeof(long);

  statbuf->st_blksize = *(blksize_t*) buf;    buf += sizeof(blksize_t);
  statbuf->st_blocks = *(blkcnt_t*)   buf;    buf += sizeof(blkcnt_t);
}

int client_getattr(const char *path, struct stat *statbuf)
{
  char r_path[MAXLEN];
  int ret_status = SUCC;

  LOG (FUSE_DATA->log, "Issuing getattr on %s\t %s\n", path, r_path);

  char buf[MAX_MESSAGE_SIZE]; memset(buf, 0, MAX_MESSAGE_SIZE);
  add_token (buf, SYS_GETATTR);
  add_token (buf, path);

  write (FUSE_DATA->sfd, buf, strlen(buf) +1); //TODO FullyWrite
  read (FUSE_DATA->sfd, buf, MAX_MESSAGE_SIZE); //TODO FullyRead

  ret_status = *(int*)buf;
  buf_to_stat (buf + sizeof(int) +1, statbuf);

  LOG (FUSE_DATA->log, "Completed getattr \n");
  return ret_status;
}

int client_open(const char *path, struct fuse_file_info *fi)
{
  char r_path[MAXLEN];
  int ret_val = SUCC;
  int fd;

  gen_real_path (r_path, path);
  LOG (FUSE_DATA->log, "Issuing open on %s\t %s\n", path, r_path);

  fd = open (r_path, fi->flags);
  if (fd < 0)
    ret_val = ERR;
  fi->fh = fd;

  LOG (FUSE_DATA->log, "Completed open\n");
  return ret_val;
}

int client_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
  char r_path[MAXLEN];
  int ret_val = 0;

  gen_real_path (r_path, path);
  LOG (FUSE_DATA->log, "Issuing read on %s\t%s\n", path, r_path);
  ret_val = pread (fi->fh, buf, size, offset);

  LOG (FUSE_DATA->log, "Completed read\n");
  return ret_val;
}

int client_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    printf("invoking write\n");
    return SUCC;
}

int client_mkdir(const char *path, mode_t mode)
{
    printf("invoking mkdir\n");
    return SUCC;
}

int client_rmdir(const char *path)
{
    printf("invoking rmdir\n");
    return SUCC;
}

int client_opendir(const char *path, struct fuse_file_info *fi)
{
  DIR* dir_ptr;
  int ret_val = SUCC;

  LOG (FUSE_DATA->log, "Issuing opendir on %s\t%s\n", path, r_path);

  dir_ptr = opendir (r_path);
  if (dir_ptr == NULL)
    ret_val = ERR;
  fi->fh = (uint64_t)dir_ptr;

  LOG (FUSE_DATA->log, "Completed opendir \n");
  return ret_val;
}

int client_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
  DIR* dir_ptr;
  char r_path[MAXLEN];
  struct dirent* dir_ent;
  int ret_val = SUCC;

  gen_real_path (r_path, path);
  LOG (FUSE_DATA->log, "Issuing opendir on %s\t%s\n", path, r_path);

  dir_ptr = (DIR*) fi->fh;
  while ((dir_ent = readdir (dir_ptr)) != NULL){
    filler (buf, dir_ent->d_name, NULL, 0);
  }

  LOG (FUSE_DATA->log, "completed readdir\n");
  return ret_val;
}

int client_unlink(const char *path)
{
  printf("invoking unlink\n");
  return SUCC;
}

int client_release(const char *path, struct fuse_file_info *fi)
{
    printf("invoking release\n");
    return SUCC;
}

int client_releasedir(const char *path, struct fuse_file_info *fi)
{
    printf("invoking releasedir\n");
    return SUCC;
}

int client_rename(const char *path, const char *newpath)
{
    printf("invoking rename\n");
    return SUCC;
}

void client_destroy(void *userdata)
{
  close(FUSE_DATA->sfd);
  LOG (FUSE_DATA->log, "Fuse finished working\n");
}

// arguments(dest, source)
void gen_real_path(char* real_path, const char* fuse_path)
{
  strcpy (real_path, FUSE_DATA->start_dir);
  strcat (real_path, fuse_path);
}
