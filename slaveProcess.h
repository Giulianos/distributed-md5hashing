#ifndef SLAVEPROCESS_H
#define SLAVEPROCESS_H

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include "queue.h"

#define HASHSIZE 32
#define MAX_FILENAME_LEN 256


void processTask(task_t * task);
taskQueue_t processAllTasks(taskQueue_t q);

#endif
