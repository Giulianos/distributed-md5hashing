#ifndef TASK_H
#define TASK_H

#include <stdint.h>

typedef struct {
  char * filename;
  uint64_t filesize;
  uint8_t processed;
  char hashmd5[33];
} task_t;

#endif
