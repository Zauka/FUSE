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
  char buf[1024];
  sfd = socket(AF_INET, SOCK_STREAM, 0);
  inet_pton(AF_INET, "127.0.0.1", &ip);

  addr.sin_family = AF_INET;
  addr.sin_port = htons(5000);
  addr.sin_addr.s_addr = ip;

  connect(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
  char* buffer = strdup ("are\nyou\nkidding\nme\n?\n\n\n\n\n\n\n\n\n\n\n\n\n");
  write(sfd, buffer, strlen(buffer)+1);
  read(sfd, &buf, 1024);
  printf ("client recieved %s\n", buf);
  close(sfd);

  LOG (FUSE_DATA->log, "Logfile is %s\n", FUSE_DATA->logfile);
  return FUSE_DATA;
}

int client_readlink(const char *path, char *link, size_t size)
{
  printf("invoking readlink\n");
  return SUCC;
}

int client_getattr(const char *path, struct stat *statbuf)
{
  char r_path[MAXLEN];
  int ret_val;

  gen_real_path (r_path, path); // generate real path
  LOG (FUSE_DATA->log, "Issuing opendir on %s\t %s\n", path, r_path);

  ret_val = lstat (r_path, statbuf);

  LOG (FUSE_DATA->log, "Completed opendir \n");
  return ret_val;
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
  char r_path[MAXLEN];
  int ret_val = SUCC;

  gen_real_path (r_path, path);
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
  LOG (FUSE_DATA->log, "Fuse finished working\n");
}

// arguments(dest, source)
void gen_real_path(char* real_path, const char* fuse_path)
{
  strcpy (real_path, FUSE_DATA->start_dir);
  strcat (real_path, fuse_path);
}
