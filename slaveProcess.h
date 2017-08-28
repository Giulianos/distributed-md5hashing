#ifndef SLAVEPROCESS_H
#define SLAVEPROCESS_H

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include "queue.h"

void processTask(task_t * task);
taskQueue_t processAllTasks(taskQueue_t q);

#endif
