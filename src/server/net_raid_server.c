#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "net_raid_server.h"
#include "../utils/protoc.h"
#include "../utils/macros.h"

#define BACKLOG 10

char* handle_names[MAX_OPEN_FILES];
int64_t handle_values[MAX_OPEN_FILES];
char start_dir[MAXLEN];

/*
*   this server implementation taken from provided socket connection example
*   from OS_2018_Spring, link : https://classroom.google.com/u/0/c/MTIxNTI5NDEyODZa
*/



// finds and returns the index
int v_find (char *name, char *handles[])
{
  for (int i=MAX_OPEN_FILES-1; i>-1; i--)
  {
    if (handles[i] == NULL)
      continue;
    // printf ("here for %d\n", i);
    // fflush (stdout);
    if (strcmp (name, handles[i]) == 0)
      return i;
  }

  return NONE;
}


// searches for NULL and fills that space (with memory allocation)
int v_add (char* name, char *handles[])
{
  for (int i=0; i<MAX_OPEN_FILES; i++)
  {
    printf ("handles[%d] = %p", i, handles[i]);
    if (handles[i] == NULL){
      handles[i] = strdup (name);
      return i;
    }
  }

  return NONE;
}

// frees memory and sets NULL
void v_remove (char* name, char* handles[])
{
  for (int i=0; i<MAX_OPEN_FILES; i++)
  {
    if (strcmp (name, handles[i]) == 0){
      free (handles[i]);
      handles[i] = NULL;
    }
  }
}


char* gen_real_path (const char *path)
{
  char* r_path = malloc(MAXLEN * sizeof(char));
  strcpy (r_path, start_dir);
  strcat (r_path, path);

  return r_path;
}


// returns number of chars written on buffer
int stat_to_buf (struct stat *statbuf, char *buf)
{
  int offset = 0;
  offset += add_token (&statbuf->st_mode, sizeof(statbuf->st_mode), buf + offset);
  offset += add_token (&statbuf->st_ino, sizeof(statbuf->st_ino), buf + offset);
  offset += add_token (&statbuf->st_dev, sizeof(statbuf->st_dev), buf + offset);
  offset += add_token (&statbuf->st_rdev, sizeof(statbuf->st_rdev), buf + offset);
  offset += add_token (&statbuf->st_nlink, sizeof(statbuf->st_nlink), buf + offset);
  offset += add_token (&statbuf->st_uid, sizeof(statbuf->st_uid), buf + offset);
  offset += add_token (&statbuf->st_gid, sizeof(statbuf->st_gid), buf + offset);
  offset += add_token (&statbuf->st_size, sizeof(statbuf->st_size), buf + offset);

  offset += add_token (&statbuf->st_atim.tv_sec, sizeof(statbuf->st_atim.tv_sec), buf + offset);
  offset += add_token (&statbuf->st_atim.tv_nsec, sizeof(statbuf->st_atim.tv_nsec), buf + offset);
  offset += add_token (&statbuf->st_mtim.tv_sec, sizeof(statbuf->st_mtim.tv_sec), buf + offset);
  offset += add_token (&statbuf->st_mtim.tv_nsec, sizeof(statbuf->st_mtim.tv_nsec), buf + offset);
  offset += add_token (&statbuf->st_ctim.tv_sec, sizeof(statbuf->st_ctim.tv_sec), buf + offset);
  offset += add_token (&statbuf->st_ctim.tv_nsec, sizeof(statbuf->st_ctim.tv_nsec), buf + offset);

  offset += add_token (&statbuf->st_blksize, sizeof(statbuf->st_blksize), buf + offset);
  offset += add_token (&statbuf->st_blocks, sizeof(statbuf->st_blocks), buf + offset);

  return offset;
}

char* fill_args (int fd, char* sysname)
{
  char* args = NULL;
  int offset = 0;

  if (strcmp (sysname, SYS_GETATTR) == 0){        // Just last NULL character
    args = calloc (sizeof(N_DELIM), sizeof(char));
    strict_read (fd, args, sizeof(N_DELIM));

  } else if (strcmp (sysname, SYS_OPEN) == 0) {
    args = calloc (MAX_MESSAGE_SIZE, sizeof(char));
    offset += strict_read (fd, args + offset, sizeof(int) + sizeof(C_DELIM));
    offset += strict_read (fd, args + offset, sizeof(N_DELIM));

  } else if (strcmp (sysname, SYS_READ) == 0) {
    args = calloc (MAX_MESSAGE_SIZE, sizeof(char));
    offset += strict_read (fd, args + offset, sizeof(size_t) + sizeof(C_DELIM));
    offset += strict_read (fd, args + offset, sizeof(off_t) + sizeof(C_DELIM));
    offset += strict_read (fd, args + offset, sizeof(N_DELIM));

  } else if (strcmp (sysname, SYS_OPENDIR) == 0) {  // Just last NULL character
    args = calloc (sizeof(N_DELIM), sizeof(char));
    strict_read (fd, args, sizeof(N_DELIM));

  } else if (strcmp (sysname, SYS_READDIR) == 0) {
    args = calloc (MAX_MESSAGE_SIZE, sizeof(char));
    offset += strict_read (fd, args + offset, sizeof(off_t) + sizeof(C_DELIM));
    offset += strict_read (fd, args + offset, sizeof(N_DELIM));

  }

  return args;
}

int msg_analyze (char *msg_prefix, int cfd)
{
  printf ("entered msg analyze with msg: %s\n", msg_prefix);

  int ret_status;
  char *buf = calloc (MAX_MESSAGE_SIZE, sizeof(char));  // initialized with NULLs so that every
  int buf_offset = 0;                                   // buf message will be NULL terminating

  char *syscall_name = next_string_token (&msg_prefix); // returns before \0 string and moves msg pointer to that \0
  char *path = next_string_token (&msg_prefix);
  char *r_path = gen_real_path (path);

  // aawyos sxva argumentebi
  char *sys_args = fill_args (cfd, syscall_name);

  printf("syscall_name: %s\npath: %s\nr_path: %s\n", syscall_name, path, r_path);

  // all cases need to call correct system call :) TODO::REMINDER
  if (strcmp (syscall_name, SYS_READLINK) == 0) {

    // char *link = next_token (&sys_args); (void)link;
    // size_t val = (size_t) sys_args; (void)val;
  } else if (strcmp (syscall_name, SYS_GETATTR) == 0) {
    struct stat statbuf;

    ret_status = lstat (r_path, &statbuf);

    // Construct buffer to send back
    buf_offset += add_token (&ret_status, sizeof(ret_status), buf + buf_offset);
    buf_offset += stat_to_buf (&statbuf, buf + buf_offset);

    strict_write (cfd, buf, buf_offset +1);
  } else if (strcmp (syscall_name, SYS_OPEN) == 0) {
    int flags = *(int*) next_token (&sys_args, sizeof(int)); (void) flags;
    int fd;

    fd = open (r_path, flags); // O_RDWR temporray?
    int idx = v_add (path, handle_names);
    handle_values [idx] = fd;

    if (fd < 0)
      ret_status = ERR;
    else
      ret_status = SUCC;

    buf_offset += add_token (&ret_status, sizeof(ret_status), buf + buf_offset);

    strict_write (cfd, buf, buf_offset +1);

  } else if (strcmp (syscall_name, SYS_READ) == 0) {

    size_t size = *(size_t*) next_token (&sys_args, sizeof(size_t));
    off_t offset = *(off_t*) next_token (&sys_args, sizeof(off_t));

    int idx = v_find (path, handle_names);
    int handle = handle_values [idx];

    char *buf = calloc  (sizeof(int) + sizeof(C_DELIM) +
                        size + sizeof(C_DELIM) + sizeof(N_DELIM),
                        sizeof(char));

    int num_read = pread (handle, buf + sizeof(int) + sizeof(C_DELIM), size, offset);
    buf [sizeof(int) + sizeof(C_DELIM) + num_read] = C_DELIM;
    memcpy (buf, &num_read, sizeof(int));
    buf [sizeof(int)] = C_DELIM;

    buf_offset = sizeof(int) + sizeof(C_DELIM) + num_read + sizeof(C_DELIM);
    strict_write(cfd, buf, buf_offset +1);

  } else if (strcmp (syscall_name, SYS_WRITE) == 0) {

    char *buf = next_string_token (&sys_args); (void)buf;
    size_t size = (size_t) next_token (&sys_args, sizeof(size_t)); (void)size;
    off_t offset = (off_t) next_token (&sys_args, sizeof(off_t)); (void)offset;
    int file_handle = v_find (path, handle_names); (void)file_handle;

  } else if (strcmp (syscall_name, SYS_MKDIR) == 0) {

    mode_t mode = (int64_t) next_token (&sys_args, sizeof(mode_t)); (void)mode;
  } else if (strcmp (syscall_name, SYS_READDIR) == 0) {
    DIR* dir_ptr;
    struct dirent* dir_ent;

    int idx = v_find (path, handle_names);
    if (idx == NONE)
      printf ("couldn't find directory\n");
    dir_ptr = (DIR*) handle_values [idx];

    buf_offset += sizeof(int) + sizeof(C_DELIM); // reserve space for length
    while ((dir_ent = readdir (dir_ptr)) != NULL){
      char *dirname = dir_ent->d_name;
      printf ("curdir = %s\n", dirname);
      buf_offset += add_string_token (dirname, strlen(dirname), buf + buf_offset);
    }
    buf_offset += add_string_token ("", strlen(""), buf + buf_offset);

    int len = buf_offset - sizeof(int) - sizeof(C_DELIM);
    memcpy (buf, &len, sizeof(int));
    buf[sizeof(int)] = C_DELIM;

    printf ("(1%% inaccurate)Sending content: %s\n", buf);
    strict_write (cfd, buf, buf_offset +1);
  } else if (strcmp (syscall_name, SYS_RMDIR) == 0) {


  } else if (strcmp (syscall_name, SYS_OPENDIR) == 0) {
    DIR* dir_ptr;
    // struct dirent* dir_ent; //DEL

    dir_ptr = opendir (r_path);
    if (dir_ptr == NULL){
      ret_status = ERR;
      printf ("ret status is ERR %d\n", ret_status);
    } else{
      ret_status = SUCC;
      printf ("ret status is SUCC %d\n", ret_status);
    }
    int idx = v_add (path, handle_names);
    handle_values[idx] = (int64_t)dir_ptr;

    buf_offset += add_token (&ret_status, sizeof(ret_status), buf + buf_offset);

    printf ("dir ptr %p, idx %d\n", dir_ptr, idx);
    strict_write (cfd, buf, buf_offset +1);

  } else if (strcmp (syscall_name, SYS_UNLINK) == 0) {


  } else if (strcmp (syscall_name, SYS_RELEASE) == 0) {

    int file_handle = (int64_t) next_token (&sys_args, sizeof(int)); (void)file_handle;
  } else if (strcmp (syscall_name, SYS_RELEASEDIR) == 0) {

    int dir_handle = (int64_t) next_token (&sys_args, sizeof(int)); (void)dir_handle;
  } else if (strcmp (syscall_name, SYS_RENAME) == 0) {

    char *new_path = gen_real_path (next_string_token (&sys_args)); (void)new_path;
  }
  return SUCC;
}

// does memory allocation
char* read_common_prefix (int cfd)
{
  char *buf = calloc (MAX_MESSAGE_SIZE, sizeof(char));
  int offset = 0;
  int len;

  printf ("read_common_prefix just started\n");
  offset += strict_read (cfd, buf + offset, sizeof(int) +1);
  len = *(int*) (buf + offset - sizeof(int) -1);
  offset += strict_read (cfd, buf + offset, len +1);
  printf ("string is %s\n", buf + offset - len -1);

  offset += strict_read (cfd, buf + offset, sizeof(int) +1);
  len = *(int*) (buf + offset - sizeof(int) -1);
  offset += strict_read (cfd, buf + offset, len +1);
  printf ("string is %s\n", buf + offset - len -1);
  buf [offset] = '\0';

  return buf;
}

void client_handler(int cfd) {

  while (1) {
    char* msg_prefix = read_common_prefix (cfd);
    printf ("this fucking common prefix: %s\n", msg_prefix);
    fflush (stdout);
    if (strcmp (msg_prefix, "") == 0 ||
        strcmp (msg_prefix, "exit") == 0)
      break; // TODO remove this clause?
    msg_analyze (msg_prefix, cfd);
    printf ("Finished this syscall\n\n");
    fflush(stdout);
  }
  close(cfd);
}

int main(int argc, char* argv[])
{
  if (argc < 2){
    fprintf(stderr, "Missing start directory!\n");
    exit(ERR);
  }

  strcpy (start_dir, argv[1]); // TODO correct me if I'm wrong

  int sfd, cfd;
  struct sockaddr_in addr;
  struct sockaddr_in peer_addr;
  int port = 5000;

  sfd = socket(AF_INET, SOCK_STREAM, 0);
  int optval = 1;
  setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
  listen(sfd, BACKLOG);

  unsigned int peer_addr_size = sizeof(struct sockaddr_in);
  cfd = accept(sfd, (struct sockaddr *) &peer_addr, &peer_addr_size);
  // SOME SOCKET STUFF UP TO THIS POINT

  for (int i=0; i<MAX_OPEN_FILES; i++)
  {
    handle_names[i] = NULL;
    handle_values[i] = -1;
  } // Initialization :: Here I'll store open file descriptors

  printf ("\tClient Handler Started\n");
  client_handler(cfd);
}
