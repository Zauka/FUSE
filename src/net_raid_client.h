#ifndef __net_raid_client__
#define __net_raid_client__

#include "macros.h"

// we have ONLY ONE disk (forget about the second one atm)
struct conf_struct{
  char logfile[MAXLEN];
  
  char diskname[MAXLEN];
  char mount[MAXLEN];
  char servers[8][MAXLEN]; // upper limit of servers = 8
  char hotswap[MAXLEN];
  int raid;
};

#endif
