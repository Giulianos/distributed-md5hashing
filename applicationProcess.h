#ifndef APLICATIONPROCESS_H
#define APLICATIONPROCESS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include "queue.h"
#include "sharedMemory.h"

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

worker_t * createWorkers(int quantity);
void pollWorkers(worker_t * workers, int quantity, taskQueue_t queue, int * processedTasks,int id_sem);
int readFromWorker(const worker_t * worker, char * buffer, int len);
int writeToWorker(const worker_t * worker, const char * msg, int len);
void assignTaskToWorker(worker_t * worker, const task_t * task);
void dispatchTasks(worker_t * workers, int quantity, taskQueue_t unassignedTasks);
void stopWorkers(const worker_t * workers, int quantity);
int readLineFromWorker(const worker_t * worker, char * buffer, int maxlen);
int fetchTasks(taskQueue_t queue, int argc, char * argv[]);
int regularFileCheck(const char * filename);


#endif
