#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct taskQueueNode taskQueueNode_t;

struct taskQueueNode
{
  task_t task;
  taskQueueNode_t * next;
};

struct taskQueueCDT
{
  taskQueueNode_t * first;
  taskQueueNode_t * last;
  int size;
};
typedef struct taskQueueCDT * taskQueue_t;

taskQueue_t newTaskQueue()
{
  taskQueue_t ret = (taskQueue_t)malloc(sizeof(struct taskQueueCDT));
  ret->first = NULL;
  ret->last = NULL;
  ret->size = 0;
  return ret;
}

void offer(taskQueue_t q, task_t task)
{
  taskQueueNode_t * newNode = (taskQueueNode_t *)malloc(sizeof(taskQueueNode_t));
  newNode->task = task;
  if(q->first == NULL)
  {
    q->first = newNode;
    q->last = newNode;
  }
  else
  {
    q->last->next = newNode;
    q->last = newNode;
  }
  q->size++;
}

task_t * poll(taskQueue_t q)
{
  if(q->first == NULL)
    return NULL;
  task_t * ret = (task_t *)malloc(sizeof(task_t));
  *ret = q->first->task;
  q->first = q->first->next;
  return ret;
}

uint8_t isEmpty(taskQueue_t q)
{
  if (q->first == NULL)
    return 1;
  else
    return 0;
}
