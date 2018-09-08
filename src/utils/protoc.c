#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "protoc.h"
#include "macros.h"


// returns the number of characters writtem on buffer
// always size +1
int add_token (const void *token, int size, char* buf)
{
  const char *tkn = (char*) token;

  for (int i=0; i<size; i++)
    buf[i] = tkn[i];
  buf [size] = C_DELIM; // adding delimiter
  buf [size+1] = '\0';  // adding terminating NULL character

  return size +1;
}

// returns the number of characters written on buffer
int add_string_token (const void* token, int size, char* buf)
{
  int offset = 0;
  offset += add_token (&size, sizeof(int), buf + offset);
  offset += add_token (token, size, buf + offset);

  return offset;
}


// Actually does len + 1 increment, cause of C_DELIM
// Changing C_DELIM to '\0'
char* next_token (char **msg, int len)
{
  char* ret_value = *msg; //(void) *msg;

  *msg += len;
  // if (**msg != C_DELIM){
  //   fprintf(stderr, "C_DELIM expected!!\n");
  //   exit(-1);
  // }
  **msg = '\0'; // change C_DELIM with NULL
  *msg += 1;    // to jump over C_DELIM, where new token is starting

  return ret_value;
}

char* next_string_token (char **msg)
{
  char *ret_value;

  int* len = (int*) next_token (msg, sizeof(int));
  ret_value = next_token (msg, *len);

  return ret_value;
}

// this function reads exactly len bytes from
// fd file descriptor
int strict_read (int fd, void* v_buf, int len)
{
  char* buf = (char*) v_buf; // Casting to char*
  int filled = 0;

  while (filled < len)
  {
    filled += read (fd, buf + filled, len - filled);
  }

  if (filled != len)
    exit(-1);
  return filled;
}


// this function writes exactly len bytes to
// fd file descriptor
void strict_write (int fd, char* buf, int len)
{
  int filled = 0;

  while (filled < len)
  {
    filled += write (fd, buf + filled, len - filled);
  }
}
