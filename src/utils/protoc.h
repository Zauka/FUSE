#ifndef __protoc__
#define __protoc__

// returns the number of characters writtem on buffer
// always size +1
int add_token (const void *token, int size, char* buf);
// returns the number of characters written on buffer
// always sizeof(int) + sizeof(C_DELIM) + size + sizeof(C_DELIM)
int add_string_token (const void* token, int size, char* buf);

// Actually does len + 1 increment, cause of C_DELIM
// Changing C_DELIM to '\0'
// returning *msg pointer
char* next_token (char **msg, int len);
// reads 2 tokens:  (1) len of string
//                  (2) string
// increments *msg pointer to the next token
// (increment = sizeof(int) + sizoef(C_DELIM) + len + sizeof(C_DELIM))
char* next_string_token (char **msg);

// this function reads exactly len bytes from
// fd file descriptor
int strict_read (int fd, void* v_buf, int len);
// this function writes exactly len bytes to
// fd file descriptor
void strict_write (int fd, char* buf, int len);

#endif //__protoc__
