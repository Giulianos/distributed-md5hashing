#include "applicationProcess.h"

int main(int argc, char * argv[])
{
  taskQueue_t q = newTaskQueue();
  uint64_t maxSize = 0;
  FILE * auxFile;
  int i;

  /* Lets create the slaves */
  umask(0);
  slave_t slaves[SLAVE_QUANTITY];
  printf("Creating slaves...\n");
  for(i=0; i<SLAVE_QUANTITY; i++)
  {
    slaves[i].id = i;
    slaves[i].mwsr = calloc(14,sizeof(char));
    sprintf(slaves[i].mwsr, ".fifomwsr%d", i);
    slaves[i].mrsw = calloc(14,sizeof(char));
    sprintf(slaves[i].mrsw, ".fifomrsw%d", i);
    mknod(slaves[i].mwsr, S_IFIFO|0666, 0);
    mknod(slaves[i].mrsw, S_IFIFO|0666, 0);
    int fork_result;
    if((fork_result=fork()) == -1)
    {
      perror(" Error creating slaves!\n");
      wait(NULL);
      exit(1);
    } else if(!fork_result) {
      //Child process (execute slave)
      printf(" Executing slave %d!\n", i);
      execlp(SLAVE_PROCESS_NAME, SLAVE_PROCESS_NAME, slaves[i].mrsw, slaves[i].mwsr, NULL);
      printf(" Error executing slave!\n");
    } else {
      printf(" Slave %d created!\n", i);
    }
  }

  /* Lets wait for the slaves to be ready */
  char * auxbuf = calloc(MAX_COMMAND_LENGTH, sizeof(char));
  printf("\nWaiting for slaves to be ready...\n");
  for(i=0; i<SLAVE_QUANTITY; i++)
  {
    auxFile = fopen(slaves[i].mrsw, "r");
    fgets(auxbuf, MAX_COMMAND_LENGTH, auxFile);
    fclose(auxFile);
    if(!strncmp(auxbuf, "ready", MAX_COMMAND_LENGTH))
    {
      printf(" Slave %d is ready!\n", slaves[i].id);
    }
  }

  /* Lets create the tasks queue and fetch file sizes */
  struct stat auxStat;
  task_t auxTask;

  for(i=1; i<argc; i++)
  {
    auxTask.filename = argv[i];
    auxTask.processed = 0;
    auxFile = fopen(argv[i], "r");
    stat(argv[i], &auxStat);
    auxTask.filesize = auxStat.st_size;
    if (auxTask.filesize > maxSize)
      maxSize = auxTask.filesize;
    offer(q, auxTask);
    printf("el tamano de %s es %d\n", auxTask.filename, auxTask.filesize);
  }
  payload_t pls[3];
  maxSize*=10;
  pls[0] = newPayload(maxSize);
  pls[1] = newPayload(maxSize);
  pls[2] = newPayload(maxSize);
  task_t * aux;
  int j=0;
  uint8_t updated = 0;
  while(!isEmpty(q))
  {
    aux = poll(q);
    updated = 0;
    while(!updated)
    {
      updated = add(pls[j], *aux);
      if(!updated)
      {
        sendPayloadToSlave(pls[j], &slaves[j]);
        if(j == 2)
          j = 0;
        else
          j++;
      }
    }
  }
  sendPayloadToSlave(pls[0], &slaves[0]);
  sendPayloadToSlave(pls[1], &slaves[1]);
  sendPayloadToSlave(pls[2], &slaves[2]);
  // while(add(pl, *(aux=poll(q))))
  // {
  //   printf("Se pudo agregar el archivo %s al payload!\n", aux->filename);
  // }
  // printf("Ya no entran mas archivos, sobraron %lu bytes\n", freeSpace(pl));
}

void sendPayloadToSlave (payload_t pl, const slave_t * sl)
{
  FILE * auxFIFO;
  task_t * auxTask;
  auxFIFO = fopen(slaves[i].mwsr, "w");
  fputs("<beginPayload>\n");
  printf("el esclavo %d esta procesando los siguientes archivos:\n", sl->id);
  while(!isEmptyPL(pl))
  {
    auxTask = get(pl);
    fputs("%s\n", auxTask->filename);
    printf("%s\n", auxTask->filename);
  }
  fputs("<endPayload>\n");
  fclose(auxFIFO);
}
