#ifndef QUEUE_H
#define QUEUE_H

#include "task.h"

typedef struct taskQueueCDT *taskQueue_t;

	/* Return a pointer to an empty stack. */
	extern taskQueue_t newTaskQueue();
	/* Push value onto the stack, returning success flag. */
	extern void offer(taskQueue_t q, task_t task);
	/* Pop value from the stack, returning success flag. */
	extern task_t * poll(taskQueue_t q);
	extern uint8_t isEmpty(taskQueue_t q);


#endif
