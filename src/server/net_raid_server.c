#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include "net_raid_server.h"
#include "../utils/macros.h"

#define BACKLOG 10

char* handle_names[MAX_OPEN_FILES];
int64_t handle_values[MAX_OPEN_FILES];
char start_dir[MAXLEN];

/*
*   this server implementation taken from provided socket connection example
*   from OS_2018_Spring, link : https://classroom.google.com/u/0/c/MTIxNTI5NDEyODZa
*/

char* next_token (char **msg)
{
  char* ret_value = *msg; //(void) *msg;

  while (**msg != C_DELIM)
    *msg += 1;

  **msg = '\0'; // change C_DELIM with NULL
  *msg += 1;    // to jump over C_DELIM, where new token is starting

  return ret_value;
}

// finds and returns the index
int v_find (char *name, char *handles[])
{
  for (int i=0; i<MAX_OPEN_FILES; i++)
  {
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

// buffer ALWAYS should be NULL terminating string
// during FIRST INVOCATION TOO!
void add_token (void *token, int size, char* buf)
{
  char* tkn = token;
  int cur_len = strlen (buf);
  buf = buf + cur_len; // now buffer is pointing to ending NULL character

  for (int i=0; i<size; i++)
    buf[i] = tkn[i];
  buf [size] = C_DELIM; // adding delimiter
  buf [size+1] = '\0';  // adding terminating NULL character
}

void stat_to_buf (struct stat *statbuf, char *buf)
{
  add_token (&statbuf->st_mode, sizeof(statbuf->st_mode), buf);
  add_token (&statbuf->st_ino, sizeof(statbuf->st_ino), buf);
  add_token (&statbuf->st_dev, sizeof(statbuf->st_dev), buf);
  add_token (&statbuf->st_rdev, sizeof(statbuf->st_rdev), buf);
  add_token (&statbuf->st_nlink, sizeof(statbuf->st_nlink), buf);
  add_token (&statbuf->st_uid, sizeof(statbuf->st_uid), buf);
  add_token (&statbuf->st_gid, sizeof(statbuf->st_gid), buf);
  add_token (&statbuf->st_size, sizeof(statbuf->st_size), buf);

  add_token (&statbuf->st_atim.tv_sec, sizeof(statbuf->st_atim.tv_sec), buf);
  add_token (&statbuf->st_atim.tv_nsec, sizeof(statbuf->st_atim.tv_nsec), buf);
  add_token (&statbuf->st_mtim.tv_sec, sizeof(statbuf->st_mtim.tv_sec), buf);
  add_token (&statbuf->st_mtim.tv_nsec, sizeof(statbuf->st_mtim.tv_nsec), buf);
  add_token (&statbuf->st_ctim.tv_sec, sizeof(statbuf->st_ctim.tv_sec), buf);
  add_token (&statbuf->st_ctim.tv_nsec, sizeof(statbuf->st_ctim.tv_nsec), buf);

  add_token (&statbuf->st_blksize, sizeof(statbuf->st_blksize), buf);
  add_token (&statbuf->st_blocks, sizeof(statbuf->st_blocks), buf);
}

int msg_analyze (char *msg, int cfd)
{
  printf ("entered msg analyze with msg: %s\n", msg);
  int ret_status;
  char buf[MAX_MESSAGE_SIZE];
  memset (buf, '\0', MAX_MESSAGE_SIZE); // Filling with NULLs

  char *syscall_name = next_token (&msg); // returns before \0 string and moves msg pointer to that \0
  char *path = next_token (&msg);
  char *r_path = gen_real_path (path);

  printf("syscall_name: %s\npath: %s\nr_path: %s\n", syscall_name, path, r_path);

  // all cases need to call correct system call :) TODO::REMINDER
  if (strcmp (syscall_name, SYS_READLINK) == 0) {

    char *link = next_token (&msg); (void)link;
    size_t val = (size_t) msg; (void)val;
  } else if (strcmp (syscall_name, SYS_GETATTR) == 0) {
    printf ("31\n");
    struct stat statbuf;

    ret_status = lstat (r_path, &statbuf);

    printf ("32\n");
    // Construct buffer to send back
    add_token (&ret_status, sizeof(ret_status), buf);
    stat_to_buf (&statbuf, buf);

    write (cfd, buf, strlen(buf) +1);

  } else if (strcmp (syscall_name, SYS_OPEN) == 0) {

    // am etapze marto file_handle mchirdeba
    int file_handle = v_find (path, handle_names); (void)file_handle;
  } else if (strcmp (syscall_name, SYS_READ) == 0) {

    size_t size = (size_t) next_token (&msg);
    off_t offset = (off_t) next_token (&msg);
    int file_handle = v_find (path, handle_names);

    char *buf = malloc(size * sizeof(char));
    int num_read = pread (file_handle, buf, size, offset);

    write(cfd, buf, num_read);

  } else if (strcmp (syscall_name, SYS_WRITE) == 0) {

    char *buf = next_token (&msg); (void)buf;
    size_t size = (size_t) next_token (&msg); (void)size;
    off_t offset = (off_t) next_token (&msg); (void)offset;
    int file_handle = v_find (path, handle_names); (void)file_handle;

  } else if (strcmp (syscall_name, SYS_MKDIR) == 0) {

    mode_t mode = (int64_t) next_token (&msg); (void)mode;
  } else if (strcmp (syscall_name, SYS_READDIR) == 0) {

    // TODO: yvela dir_ent-is saxeli unda chamowero da socket-shi gaagzavno
  } else if (strcmp (syscall_name, SYS_RMDIR) == 0) {


  } else if (strcmp (syscall_name, SYS_OPENDIR) == 0) {

    int file_handle = (int64_t) next_token (&msg); (void)file_handle;
  } else if (strcmp (syscall_name, SYS_UNLINK) == 0) {


  } else if (strcmp (syscall_name, SYS_RELEASE) == 0) {

    int file_handle = (int64_t) next_token (&msg); (void)file_handle;
  } else if (strcmp (syscall_name, SYS_RELEASEDIR) == 0) {

    int dir_handle = (int64_t) next_token (&msg); (void)dir_handle;
  } else if (strcmp (syscall_name, SYS_RENAME) == 0) {

    char *new_path = gen_real_path (next_token (&msg)); (void)new_path;
  }
  return ERR;
}

// does memory allocation
char* read_single_command (int cfd)
{
  char buf[MAX_MESSAGE_SIZE];
  int data_size;

  data_size = read (cfd, buf, 1024);
  buf [data_size] = '\0';

  return strdup (buf);
}

void client_handler(int cfd) {
    char buf[1024];
    int ret_status;

    while (1) {
      printf ("1\n");
      char* single_command = read_single_command (cfd);
      printf ("2\n");
      if (strcmp (single_command, "") == 0 ||
          strcmp (single_command, "exit") == 0)
        break;
      printf ("3\n");
      ret_status = msg_analyze (single_command, cfd);
      printf ("4\n");
      // write_int (ret_status, buf);
      add_token (&ret_status, sizeof(ret_status), buf);
      printf ("5\n");
      write (cfd, buf, sizeof(long long) +1);
      printf ("6\n");
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

  printf ("Imma hear\n");
  client_handler(cfd);
}
