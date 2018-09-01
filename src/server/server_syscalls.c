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

#include "server_syscalls.h"
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



int server_readlink(const char *path, char *link, size_t size)
{
  printf("invoking readlink\n");
  return SUCC;
}

int server_getattr(const char *path, struct stat *statbuf)
{
  char r_path[MAXLEN];
  int ret_val;

  gen_real_path (r_path, path); // generate real path
  LOG (FUSE_DATA->log, "Issuing opendir on %s\t %s\n", path, r_path);

  ret_val = lstat (r_path, statbuf);

  LOG (FUSE_DATA->log, "Completed opendir \n");
  return ret_val;
}

int server_open(const char *path)
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

int server_read(const char *path, char *buf, size_t size, off_t offset)
{
  char r_path[MAXLEN];
  int ret_val = 0;

  gen_real_path (r_path, path);
  LOG (FUSE_DATA->log, "Issuing read on %s\t%s\n", path, r_path);
  ret_val = pread (fi->fh, buf, size, offset);

  LOG (FUSE_DATA->log, "Completed read\n");
  return ret_val;
}

int server_write(const char *path, const char *buf, size_t size, off_t offset)
{
    printf("invoking write\n");
    return SUCC;
}

int server_mkdir(const char *path, mode_t mode)
{
    printf("invoking mkdir\n");
    return SUCC;
}

int server_rmdir(const char *path)
{
    printf("invoking rmdir\n");
    return SUCC;
}

int server_opendir(const char *path)
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

int server_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset)
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

int server_unlink(const char *path)
{
  printf("invoking unlink\n");
  return SUCC;
}

int server_release(const char *path)
{
    printf("invoking release\n");
    return SUCC;
}

int server_releasedir(const char *path)
{
    printf("invoking releasedir\n");
    return SUCC;
}

int server_rename(const char *path, const char *newpath)
{
    printf("invoking rename\n");
    return SUCC;
}

void server_destroy(void *userdata)
{
  LOG (FUSE_DATA->log, "Fuse finished working\n");
}

// arguments(dest, source)
void gen_real_path(char* real_path, const char* fuse_path)
{
  strcpy (real_path, FUSE_DATA->start_dir);
  strcat (real_path, fuse_path);
}
