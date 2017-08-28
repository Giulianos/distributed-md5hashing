#include <stdio.h>
#include "queue.h"
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include "slaveProcess.h"

int main(int argc, char *argv[])
{
  taskQueue_t q = newTaskQueue();
  taskQueue_t ret = newTaskQueue();
  task_t t1, t2;
  t1.filename = "prueba1";
  t1.processed = 0;
  t2.filename = "prueba2";
  t2.processed = 0;
  offer(q, t1);
  offer(q, t2);
  ret = processAllTasks(q);
  task_t * aux;
  while(!(isEmpty(ret)))
  {
    aux = poll(ret);
    printf("El hash md5 de %s es %s\n", aux->filename, aux->hashmd5);
  }

  return 0;
}

void processTask(task_t * task)
{
  int pipefd[2];
  pid_t pid;
  pipe(pipefd);

  if((pid=fork())==-1)
  {
    printf("Error fork!\n");
  }
  else if(pid==0)
  {
    /* Child */

    /* Redirecciono stdout a fd[1] (pipe input)*/
    dup2(pipefd[1], 1);
    close(pipefd[0]);
    close(pipefd[1]);
    /* Ejecuto md5sum */
    execlp("md5sum", "md5sum", task->filename, NULL);
  }
  else
  {
    /* Parent */
    uint8_t current = 0;
    /* Leo desde el pipe hasta encontrar un espacio
    (el comando md5sum devuelve [hash] [nombre_archivo]) */
    read(pipefd[0], task->hashmd5, 32);
    wait(NULL);
    task->hashmd5[32]='\0';
    task->processed = 1;
    close(pipefd[0]);
    close(pipefd[1]);
    return;
  }
}

taskQueue_t processAllTasks(taskQueue_t q)
{
  taskQueue_t ret = newTaskQueue();
  task_t * aux;
  while (!isEmpty(q))
  {
    aux = poll(q);
    processTask(aux);
    offer(ret, *aux);
  }
  return ret;
}
