#ifndef SLAVEPROCESS_H
#define SLAVEPROCESS_H

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include "queue.h"

#define WAITING_FOR_TASKS 1
#define RECEVING_TASKS 2
#define TASKS_READY 3

void processTask(task_t * task);
taskQueue_t processAllTasks(taskQueue_t q);

#endif
