#ifndef __constants__
#define __constants__

#define ERR -1
#define SUCC 0

#define NONE -1
#define MAXLEN 128
#define MAX_OPEN_FILES 300
#define MAX_MESSAGE_SIZE 1024

#define SER_STAT_SIZE 132
#define EMPTY_DIR "empty"

#define DELIMS  ",= "
#define C_DELIM ((char)'!') //TODO change back to '\1'
#define N_DELIM ((char)'\0')

#define KEY_LOGFILE "errorlog"
#define KEY_CACHE_SIZE "cache_size"
#define KEY_CACHE_REPL "cache_replacement"
#define KEY_TIMEOUT "timeout"
#define KEY_DISKNAME "diskname"
#define KEY_MOUNT "mountpoint"
#define KEY_SERVERS "servers"
#define KEY_HOTSWAP "hotswap"
#define KEY_RAID  "raid"

#define SYS_READLINK "readlink"
#define SYS_GETATTR "getattr"
#define SYS_OPEN "open"
#define SYS_READ "read"
#define SYS_WRITE "write"
#define SYS_MKDIR "mkdir"
#define SYS_READDIR "readdir"
#define SYS_RMDIR "rmdir"
#define SYS_OPENDIR "opendir"
#define SYS_UNLINK "unlink"
#define SYS_RELEASE "release"
#define SYS_RELEASEDIR "releasedir"
#define SYS_RENAME "rename"

#endif
