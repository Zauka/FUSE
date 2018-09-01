#ifndef __net_raid_client__
#define __net_raid_client__

#include "../utils/macros.h"

// we have ONLY ONE disk (forget about the second one atm)
struct conf_struct{
  char logfile[MAXLEN];
  int cache_size;
  char cache_repl[MAXLEN];
  int timeout;

  char diskname[MAXLEN];
  char mount[MAXLEN];
  char servers[8][MAXLEN]; // upper limit of servers = 8
  char hotswap[MAXLEN];
  int raid;
};

#endif
