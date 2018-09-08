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
#include "../utils/protoc.h"
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
  fflush (FUSE_DATA->log);
  return FUSE_DATA;
}

int client_readlink(const char *path, char *link, size_t size)
{
  printf("invoking readlink\n");
  return SUCC;
}

void buf_to_stat (char* buf, struct stat *statbuf)
{
  statbuf->st_mode  = *(mode_t*)  buf;        buf += 1+ sizeof(mode_t);
  statbuf->st_ino   = *(ino_t*)   buf;        buf += 1+ sizeof(ino_t);
  statbuf->st_dev   = *(dev_t*)   buf;        buf += 1+ sizeof(dev_t);
  statbuf->st_rdev  = *(dev_t*)   buf;        buf += 1+ sizeof(dev_t);
  statbuf->st_nlink = *(nlink_t*) buf;        buf += 1+ sizeof(nlink_t);
  statbuf->st_uid   = *(uid_t*)   buf;        buf += 1+ sizeof(uid_t);
  statbuf->st_gid   = *(gid_t*)   buf;        buf += 1+ sizeof(gid_t);
  statbuf->st_size  = *(off_t*)   buf;        buf += 1+ sizeof(off_t);

  statbuf->st_atim.tv_sec = *(time_t*) buf;   buf += 1+ sizeof(time_t);
  statbuf->st_atim.tv_nsec = *(long*)  buf;   buf += 1+ sizeof(long);
  statbuf->st_mtim.tv_sec = *(time_t*) buf;   buf += 1+ sizeof(time_t);
  statbuf->st_mtim.tv_nsec = *(long*)  buf;   buf += 1+ sizeof(long);
  statbuf->st_ctim.tv_sec = *(time_t*) buf;   buf += 1+ sizeof(time_t);
  statbuf->st_ctim.tv_nsec = *(long*)  buf;   buf += 1+ sizeof(long);

  statbuf->st_blksize = *(blksize_t*) buf;    buf += 1+ sizeof(blksize_t);
  statbuf->st_blocks = *(blkcnt_t*)   buf;    buf += 1+ sizeof(blkcnt_t);
}

int client_getattr(const char *path, struct stat *statbuf)
{
  int ret_status;
  char *msg = calloc (MAX_MESSAGE_SIZE, sizeof(char));
  int msg_offset = 0;

  LOG (FUSE_DATA->log, "Issuing getattr on %s\n", path);
  fflush (FUSE_DATA->log);

  msg_offset += add_string_token (SYS_GETATTR, strlen(SYS_GETATTR), msg + msg_offset);
  msg_offset += add_string_token (path, strlen(path), msg + msg_offset);
  strict_write (FUSE_DATA->sfd, msg, msg_offset +1);

  strict_read (FUSE_DATA->sfd, msg, sizeof(ret_status) + sizeof(C_DELIM) + SER_STAT_SIZE + sizeof(N_DELIM));
  ret_status = *(int*) next_token (&msg, sizeof(int));
  buf_to_stat (msg, statbuf);

  LOG (FUSE_DATA->log, "Completed getattr with status %d\n\n", ret_status);
  fflush (FUSE_DATA->log);
  return ret_status;
}

int client_open(const char *path, struct fuse_file_info *fi)
{
  int ret_status = SUCC;

  LOG (FUSE_DATA->log, "Issuing open on %s\n", path);
  fflush (FUSE_DATA->log);

  char *msg = calloc (MAX_MESSAGE_SIZE, sizeof(char));
  int msg_offset = 0;

  msg_offset += add_string_token (SYS_OPEN, strlen(SYS_OPEN), msg + msg_offset);
  msg_offset += add_string_token (path, strlen(path), msg + msg_offset);
  msg_offset += add_token (&fi->flags, sizeof(fi->flags), msg + msg_offset);
  strict_write (FUSE_DATA->sfd, msg, msg_offset +1);

  strict_read (FUSE_DATA->sfd, msg, sizeof(ret_status) + sizeof(C_DELIM) + sizeof(N_DELIM));
  ret_status = *(int*)next_token (&msg, sizeof(int));

  LOG (FUSE_DATA->log, "Completed open\n");
  fflush (FUSE_DATA->log);

  return ret_status;
}

int client_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
  // TODO what is buf size? what if it's too small? who's responsible?
  int ret_status;

  LOG (FUSE_DATA->log, "Issuing read on %s\n", path);
  fflush (FUSE_DATA->log);

  char *msg = calloc (MAX_MESSAGE_SIZE, sizeof(char));
  int msg_offset = 0;

  msg_offset += add_string_token (SYS_READ, strlen(SYS_READ), msg + msg_offset);
  msg_offset += add_string_token (path, strlen(path), msg + msg_offset);
  msg_offset += add_token (&size, sizeof(size), msg + msg_offset);
  msg_offset += add_token (&offset, sizeof(offset), msg + msg_offset);

  strict_write (FUSE_DATA->sfd, msg, msg_offset +1);

  msg_offset = 0;
  msg_offset += strict_read (FUSE_DATA->sfd, msg + msg_offset, sizeof(int) + sizeof(C_DELIM));
  int len = *(int*) (msg + offset - sizeof(int) - sizeof(C_DELIM));
  msg_offset += strict_read (FUSE_DATA->sfd, msg + msg_offset, len + sizeof(C_DELIM));
  msg_offset += strict_read (FUSE_DATA->sfd, msg + msg_offset, sizeof(N_DELIM));

  ret_status = len;

  LOG (FUSE_DATA->log, "Completed read with status %d\n\n", ret_status);
  fflush (FUSE_DATA->log);

  return ret_status;
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
  int ret_status;

  LOG (FUSE_DATA->log, "Issuing opendir on %s\n", path);
  fflush (FUSE_DATA->log);


  char *msg = calloc (MAX_MESSAGE_SIZE, sizeof(char));
  int msg_offset = 0;
  msg_offset += add_string_token (SYS_OPENDIR, strlen(SYS_OPENDIR), msg + msg_offset);
  msg_offset += add_string_token (path, strlen(path), msg + msg_offset);

  strict_write (FUSE_DATA->sfd, msg, msg_offset +1);

  msg_offset = 0;
  msg_offset += strict_read (FUSE_DATA->sfd, msg + msg_offset, sizeof(int) + sizeof(C_DELIM) + sizeof (N_DELIM));
  ret_status = *(int*)next_token (&msg, sizeof(int));

  LOG (FUSE_DATA->log, "Completed opendir with status %d\n\n", ret_status);
  fflush (FUSE_DATA->log);

  return ret_status;
}

int client_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
  int ret_status = SUCC;

  LOG (FUSE_DATA->log, "Issuing readdir on %s\n", path);
  fflush (FUSE_DATA->log);

  char *msg = calloc (MAX_MESSAGE_SIZE, sizeof(char)); // calloc does initialization to 0s
  int msg_offset = 0;

  msg_offset += add_string_token (SYS_READDIR, strlen(SYS_READDIR), msg + msg_offset);
  msg_offset += add_string_token (path, strlen(path), msg + msg_offset);
  msg_offset += add_token (&offset, sizeof(offset), msg + msg_offset);
  strict_write (FUSE_DATA->sfd, msg, msg_offset +1);

  msg_offset = 0;
  strict_read (FUSE_DATA->sfd, msg, sizeof(int) + sizeof(C_DELIM));
  int len = *(int*) msg;
  strict_read (FUSE_DATA->sfd, msg, len + sizeof(N_DELIM)); //this +1 is VITAL!!!!!

  char *d_name;
  while (1) {
    d_name = next_string_token (&msg);

    LOG (FUSE_DATA->log, "Current recieved dir: %s\n", d_name);
    fflush (FUSE_DATA->log);

    if (strcmp(d_name, "") == 0)
      break;

    filler (buf, d_name, NULL, 0);
  }

  LOG (FUSE_DATA->log, "completed readdir with status %d\n\n", ret_status);
  fflush (FUSE_DATA->log);

  return ret_status;
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
