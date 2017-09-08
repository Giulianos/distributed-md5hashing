#ifndef APLICATIONPROCESS_H
#define APLICATIONPROCESS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "slaveProcess.h"
#include "payload.h"
#include "queue.h"

#define JOBSIZE 3
#define WORKERS_QUANTITY 3
#define SLAVE_EXEC_NAME "./slave"

#define DEBUG_MSG 1

#define ASSIGN_TASK_CMD 2
#define STOP_WORKER_CMD 3

typedef struct
{
    int id;
    int pid;
    int unprocessed;
    int read_pipe;
    int write_pipe;
} worker_t;

#endif
