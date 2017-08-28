#ifndef APLICATIONPROCESS_H
#define APLICATIONPROCESS_H

#include <stdio.h>
#include "queue.h"
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include "slaveProcess.h"
#include "payload.h"

void sendPayloadToSlave (payload_t pl, int slaveNumber);

#endif
