#define FUSE_USE_VERSION 29

#include <fuse.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "net_raid_client.h"
#include "file_syscalls.h"
#include "macros.h"


static struct fuse_operations zfs_oper = {
  .init = zfs_init,
  .open		= zfs_open,
	.getattr	= zfs_getattr,
  .opendir = zfs_opendir,
	.readdir	= zfs_readdir,
	.read		= zfs_read
};


int parse_config_file (int argc, char *argv[], struct conf_struct* info)
{
  if (argc != 2){
    fprintf(stderr, "(Config parsing)\tInvalid number of arguments\n");
    return ERR;
  }

  char* config_file = argv[1];

  char* line = NULL;
  int line_len;
  size_t len = 0;

  FILE* fptr = fopen(config_file, "r");
  while ( (line_len = getline (&line, &len, fptr)) != -1)
  {
    line[line_len-1] = '\0';
    // printf("line: \"%s\"\tlen=%d\n", line, line_len);

    char* key = strtok(line, DELIMS);
    char* value = strtok(NULL, DELIMS);

    if ((key == NULL) || (strlen(key) >= 2 && key[0] == key[1] && key[1] == '/'))
      continue;

    if (strcmp (key, KEY_LOGFILE) == 0){
      strcpy (info->logfile, value);
    } else if (strcmp (key, KEY_DISKNAME) == 0){
      strcpy (info->diskname, value);
    } else if (strcmp (key, KEY_MOUNT) == 0) {
      strcpy (info->mount, value);
    } else if (strcmp (key, KEY_RAID) == 0) {
      info->raid = atoi (value);
    } else if (strcmp (key, KEY_SERVERS) == 0) {
      int i=0;
      strcpy(info->servers[i], value);
      for (i=1; (value = strtok(NULL, DELIMS)) != 0; i++)
        strcpy (info->servers[i], value);
    } else if (strcmp (key, KEY_HOTSWAP) == 0) {
      strcpy (info->hotswap, value);
    } else {
      fprintf(stderr, "(Config parsing)\tUnknown agrument: \"%s\"\n", key);
      return ERR;
    }
    // printf("key=\"%s\" \t value=\"%s\" \n", key, value);
  }
  return SUCC;
  // name, mountport, raid, servers(ip,port), hotswap
}

int main(int argc, char *argv[])
{
  printf("%d\n", getpid());
  struct conf_struct cfg;
  if (parse_config_file (argc, argv, &cfg) == ERR){
    exit(ERR);
  } else {
    printf("(Config parsing) \t Success \n");
  }

  struct fuse_info *info = malloc (sizeof (struct fuse_info));
  strcpy (info->logfile, cfg.logfile);

  char* fuse_args[3] = {argv[0], cfg.mount, NULL};
  printf("mountpoint is \"%s\"\n", fuse_args[1]);
  int ret = fuse_main(2, fuse_args, &zfs_oper, info);
  printf("fuse returned %d\n", ret);
}
