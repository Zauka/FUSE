#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "net_raid_server.h"
#include "../utils/macros.h"

#define BACKLOG 10

/*
*   this server implementation taken from provided socket connection example
*   from OS_2018_Spring, link : https://classroom.google.com/u/0/c/MTIxNTI5NDEyODZa
*/

char* next_token (char **msg, enum Mode mode)
{
  char* ret_value = *msg;

  if (mode == num_mode) {
    *msg += sizeof (long long); // every integer type should be saved as 8-byte (for simpilcity)
    *msg ++; // to jump over \0, where new token is starting
  } else if (mode == string_mode) {
    while (**msg != 0){
      *msg ++;
    }
    *msg ++; // to jump over \0, where new token is starting
  }

  return ret_value;
}

// serialize integer int its very form
void write_int (long long num, char *buf)
{
  char *ptr = (char*)&num;
  for (int i=0; i<sizeof(long long); i++) buf[i] = *(ptr++);
  buf[sizeof(long long)] = 0;
}

int v_find (char *name, char *handles[])
{
  for (int i=0; i<MAX_OPEN_FILES; i++) {
    if (strcmp (name, handles[i]) == 0)
      return i;
  }

  return NONE;
}

char* gen_real_path (char *path)
{
  return path;
}

void msg_analyze (char *msg, char* handles[], int cfd)
{
  char *syscall_name = next_token (&msg, string_mode); // returns before \0 string and moves msg pointer to that \0
  char *path = gen_real_path (next_token (&msg, string_mode));

// all cases need to call correct system call :) REMINDER
  if (strcmp (syscall_name, SYS_READLINK) == 0) {

    char *link = next_token (&msg, string_mode);
    size_t val = (size_t) msg;
  } else if (strcmp (syscall_name, SYS_GETATTR) == 0) {

    struct stat *UNUSED_statbuf = (struct stat*) next_token (&msg, num_mode); (void) UNUSED_statbuf;
  } else if (strcmp (syscall_name, SYS_OPEN) == 0) {

    // am etapze marto file_handle mchirdeba
    int file_handle = v_find (path, handles);
  } else if (strcmp (syscall_name, SYS_READ) == 0) {

    size_t size = (size_t) next_token (&msg, num_mode);
    off_t offset = (off_t) next_token (&msg, num_mode);
    int file_handle = v_find (path, handles);

    char *buf = malloc(size * sizeof(char));
    int num_read = pread (file_handle, buf, size, offset);

    write(cfd, buf, num_read);

  } else if (strcmp (syscall_name, SYS_WRITE) == 0) {

    char *buf = next_token (&msg, string_mode);
    size_t size = (size_t) next_token (&msg, num_mode);
    off_t offset = (off_t) next_token (&msg, num_mode);
    int file_handle = v_find (path, handles);
  } else if (strcmp (syscall_name, SYS_MKDIR) == 0) {

    mode_t mode = (mode_t) next_token (&msg, num_mode);
  } else if (strcmp (syscall_name, SYS_READDIR) == 0) {

    // TODO: yvela dir_ent-is saxeli unda chamowero da socket-shi gaagzavno
  } else if (strcmp (syscall_name, SYS_RMDIR) == 0) {


  } else if (strcmp (syscall_name, SYS_OPENDIR) == 0) {

    int file_handle = (int64_t) next_token (&msg, num_mode);
  } else if (strcmp (syscall_name, SYS_UNLINK) == 0) {


  } else if (strcmp (syscall_name, SYS_RELEASE) == 0) {

    int file_handle = (int64_t) next_token (&msg, num_mode);
  } else if (strcmp (syscall_name, SYS_RELEASEDIR) == 0) {

    int dir_handle = (int64_t) next_token (&msg, num_mode);
  } else if (strcmp (syscall_name, SYS_RENAME) == 0) {

    char *new_path = gen_real_path (next_token (&msg, string_mode));

  }
}

void client_handler(int cfd) {
    char buf[1024];
    int data_size;
    while (1) {
        data_size = read (cfd, &buf, 1024);
        if (data_size <= 0)
            break;
        buf [data_size] = 0;
        printf ("recieved data is %s\n with size %d\n", buf, data_size);
        write (cfd, &buf, data_size);
    }
    close(cfd);
}

int main(int argc, char* argv[])
{
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

    // switch(fork()) {
    //     case -1:
    //         exit(100);
    //     case 0:
    close(sfd);
    client_handler(cfd);
    //         exit(0);
    //     default:
    //         close(cfd);
    // }

}
