#include "aplicationProcess.h"
#include <sys/stat.h>

int main(int argc, char * argv[])
{
  task_t t;
  taskQueue_t q = newTaskQueue();
  uint64_t maxSize = 0;
  FILE * f;
  struct stat st;
  int i;
  for(i=1; i<argc; i++)
  {
    t.filename = argv[i];
    t.processed = 0;
    f = fopen(argv[i], "r");
    stat(argv[i], &st);
    t.filesize = st.st_size;
    if (t.filesize > maxSize)
      maxSize = t.filesize;
    offer(q, t);
    printf("el tamano de %s es %d\n", t.filename, t.filesize);
  }
  payload_t pls[3];
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
        sendPayloadToSlave(pls[j], j);
        if(j == 2)
          j = 0;
        else
          j++;
      }
    }
  }
  sendPayloadToSlave(pls[0], 0);
  sendPayloadToSlave(pls[1], 1);
  sendPayloadToSlave(pls[2], 2);
  // while(add(pl, *(aux=poll(q))))
  // {
  //   printf("Se pudo agregar el archivo %s al payload!\n", aux->filename);
  // }
  // printf("Ya no entran mas archivos, sobraron %lu bytes\n", freeSpace(pl));
}

void sendPayloadToSlave (payload_t pl, int slaveNumber)
{
  printf("el esclavo %d proceso los siguientes archivos:\n", slaveNumber);
  while(!isEmptyPL(pl))
  {
    printf("%s\n", get(pl)->filename);
  }
}
