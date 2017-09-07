#ifndef QUEUE_H
#define QUEUE_H

#include "task.h"

typedef struct taskQueueCDT *taskQueue_t;

	extern taskQueue_t newTaskQueue();
	extern void offer(taskQueue_t q, task_t task);
	extern task_t * poll(taskQueue_t q);
	extern int size(taskQueue_t q);
	extern uint8_t isEmpty(taskQueue_t q);

#endif
