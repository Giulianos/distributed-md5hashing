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

#define SLAVE_QUANTITY 5
#define SLAVE_PROCESS_NAME "./slave"
#define MAX_COMMAND_LENGTH 50

typedef struct
{
    int id;
    char * mwsr; //Master write - Slave read
    char * mrsw; //Master read - Slave write
} slave_t;

void sendPayloadToSlave(payload_t pl, const slave_t * sl);

#endif
