#include "aplicationProcess.h"
#include <sys/stat.h>

int main(int argc, char * argv[])
{
  task_t t;
  taskQueue_t q = newTaskQueue();
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
    offer(q, t);
    printf("el tamano de %s es %d\n", t.filename, fSize);
  }


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
