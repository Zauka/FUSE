#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <dirent.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>

#include "server_syscalls.h"
#include "net_raid_server.h"
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
  char *r_path;
  int ret_val;

  r_path = gen_real_path (path); // generate real path
  // LOG (FUSE_DATA->log, "Issuing opendir on %s\t %s\n", path, r_path);

  ret_val = lstat (r_path, statbuf);

  // LOG (FUSE_DATA->log, "Completed opendir \n");
  return ret_val;
}

int server_open(const char *path, int flags)
{
  char *r_path;
  int ret_val = SUCC;
  int fd;

  r_path = gen_real_path (path);
  // LOG (FUSE_DATA->log, "Issuing open on %s\t %s\n", path, r_path);

  fd = open (r_path, flags);
  if (fd < 0)
    ret_val = ERR;
  // TODO should add to v_handles array

  // LOG (FUSE_DATA->log, "Completed open\n");
  return ret_val;
}

int server_read(const char *path, char *buf, size_t size, off_t offset)
{
  char *r_path; (void)r_path;
  int ret_val = 0;

  r_path = gen_real_path (path);
  int file_handle = -1;
  // search for file_handle in v_handles
  ret_val = pread (file_handle, buf, size, offset);

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
  char *r_path;
  int ret_val = SUCC;

  r_path = gen_real_path (path);
  // LOG (FUSE_DATA->log, "Issuing opendir on %s\t%s\n", path, r_path);

  dir_ptr = opendir (r_path);
  if (dir_ptr == NULL)
    ret_val = ERR;
  // TODO save dir_ptr to v_handles for further usage
  // LOG (FUSE_DATA->log, "Completed opendir \n");
  return ret_val;
}

int server_readdir(const char *path, void *buf, off_t offset)
{
  DIR* dir_ptr; (void)dir_ptr;
  char *r_path; (void)r_path;
  struct dirent* dir_ent; (void)dir_ent;
  int ret_val = SUCC;

  r_path = gen_real_path (path);
  // LOG (FUSE_DATA->log, "Issuing opendir on %s\t%s\n", path, r_path);

  // dir_ptr = (DIR*) fi->fh;
  // TODO search dir_ptr in v_handles
  //      and write to buf
  // while ((dir_ent = readdir (dir_ptr)) != NULL){
  //   filler (buf, dir_ent->d_name, NULL, 0);
  // }

  // LOG (FUSE_DATA->log, "completed readdir\n");
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
  // LOG (FUSE_DATA->log, "Fuse finished working\n");
}
