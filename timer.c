//
// Created by hu on 12/27/17.
//

#include "timer.h"

#include <sys/time.h>

uint64_t stopwatch_start()
{
  struct timeval timestamp;

  gettimeofday(&timestamp, 0);

  return ((uint64_t)timestamp.tv_sec * 1000 + (uint64_t)timestamp.tv_usec / 1000);
}

uint64_t stopwatch_stop(uint64_t orig_timestamp)
{
  struct timeval timestamp;

  gettimeofday(&timestamp, 0);

  return (((uint64_t)timestamp.tv_sec * 1000 + (uint64_t)timestamp.tv_usec / 1000)) - orig_timestamp;
}
