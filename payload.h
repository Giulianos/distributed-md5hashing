#ifndef PAYLOAD_H
#define PAYLOAD_H

  #include "task.h"
  #include <stdint.h>

  typedef struct payloadCDT *payload_t;

	extern payload_t newPayload(uint64_t maxBytes);
	extern uint8_t add(payload_t p, task_t task);
  extern uint64_t freeSpace(payload_t p);

#endif
