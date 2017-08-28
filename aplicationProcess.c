#include "aplicationProcess.h"
#include <sys/stat.h>
#include "payload.h"

int main(int argc, char * argv[])
{
  task_t t;
  taskQueue_t q = newTaskQueue();

  payload_t pl = newPayload(20000);

  FILE * f;
  struct stat st;
  int i;
  for(i=0; i<argc; i++)
  {
    t.filename = argv[i];
    t.processed = 0;
    f = fopen(argv[i], "r");
    stat(argv[i], &st);
    size_t fSize = st.st_size;
    t.filesize = fSize;
    offer(q, t);
    printf("el tamano de %s es %d\n", t.filename, fSize);
  }

  task_t * aux;

  while(add(pl, *(aux=poll(q))))
  {
    printf("Se pudo agregar el archivo %s al payload!\n", aux->filename);
  }
  printf("Ya no entran mas archivos, sobraron %lu bytes\n", freeSpace(pl));

// struct stat st;
// stat(filename, &st);
// size = st.st_size;
  // taskQueue_t processed = processAllTasks(q);
  // task_t * aux;
  // while(!isEmpty(processed))
  // {
  //   aux = poll(processed);
  //   printf("El hash md5 de %s es %s\n", aux->filename, aux->hashmd5);
  // }
}
