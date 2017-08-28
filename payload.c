#include "payload.h"
#include "stdlib.h"

typedef struct payloadNode payloadNode_t;

struct payloadNode
{
  task_t task;
  payloadNode_t * next;
};

struct payloadCDT
{
  payloadNode_t * first;
  payloadNode_t * last;
  uint64_t size;
  uint64_t maxSize;
};

typedef struct payloadCDT * payload_t;

payload_t newPayload(uint64_t maxBytes)
{
  payload_t retPayload = malloc(sizeof(struct payloadCDT));

  retPayload->first = retPayload->last = NULL;
  retPayload->maxSize = maxBytes;
  retPayload->size = 0;

  return retPayload;
}

uint8_t add(payload_t p, task_t task)
{
  if(p->size + task.filesize > p->maxSize)
    return 0;

  payloadNode_t * new = malloc(sizeof(payloadNode_t));
  new->task = task;
  new->next = NULL;
  if(!p->first)
  {
    p->first = new;
    p->last = new;
  } else {
    p->last->next = new;
    p->last = new;
  }
  p->size += task.filesize;
  return 1;
}

uint64_t freeSpace(payload_t p)
{
  return p->maxSize - p->size;
}
