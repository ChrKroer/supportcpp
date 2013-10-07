#include <sys/time.h>
#include <sys/resource.h>
#include <stdlib.h>
#include "systemresourcehandler.h"

void SystemResourceHandler::SetMemoryLimit(int cap_in_mb) {
  struct rlimit memlimit;
  long bytes;

  if(cap_in_mb != -1)
    {
      bytes = cap_in_mb*(1024*1024);
      memlimit.rlim_cur = bytes;
      memlimit.rlim_max = bytes;
      setrlimit(RLIMIT_AS, &memlimit);
    }
}
