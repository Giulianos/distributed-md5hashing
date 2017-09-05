#include "slaveProcess.h"

int main(int argc, char *argv[])
{

	uint8_t running = 1;

  taskQueue_t unprocessedTasks = newTaskQueue();
  taskQueue_t processedTasks;

	task_t * auxTask;

	int filenameLen;

  //Acknowledge application
  FILE * f = fopen(argv[1], "w");
  fputs("ready", f);
  fclose(f);

	//Start reading commands
	char * commandBuffer[512];

	uint8_t state = WAITING_FOR_TASKS;

	while(running)
	{
		//Wait and read command into buffer
		f = fopen(argv[2], "r");
		fgets(commandBuffer, 512, f);
		fclose(f);
		switch(state)
		{
			case WAITING_FOR_TASKS:
				if(strncmp(commandBuffer, "<beginPayload>", 512)==0)
				{
					state = RECEVING_TASKS;
				}
				else if(strncmp(commandBuffer, "<killSlave>", 512)==0)
				{
					wait(NULL);
					_exit(0);
				}
				break;
			case RECEVING_TASKS:
				if(strncmp(commandBuffer, "<endPayload>", 512)==0)
				{
					processedTasks = processAllTasks(unprocessedTasks);
					state = TASKS_READY;
				}
				else if(strncmp(commandBuffer, "<killSlave>", 512)==0)
				{
					wait(NULL);
					_exit(0);
				}
				else
				{
					auxTask = malloc(sizeof(task_t));
					auxTask->filename = calloc(strnlen_s(c, 512), sizeof(char));
					strncpy(auxTask->filename, commandBuffer, 512);
					auxTask->processed = 0;
					offer(unprocessedTasks, *auxTask);
				}
				break;
			case TASKS_READY:
				if(strncmp(commandBuffer, "<retreiveFinishedTasks>", 512)==0)
				{
					f = fopen(argv[1], "w");
					while(!(isEmpty(processedTasks)))
				  {
				    auxTask = poll(processedTasks);
				    fprintf(f, "%s:%s\n", auxTask->filename, auxTask->hashmd5);
				  }
					fclose(f);
					state = WAITING_FOR_TASKS;
				}
				else if(strncmp(commandBuffer, "<killSlave>", 512)==0)
				{
					wait(NULL);
					_exit(0);
				}
				break;
		}
	}
  offer(q, t1);
  offer(q, t2);
  ret = processAllTasks(q);
  task_t * aux;
  while(!(isEmpty(ret)))
  {
    aux = poll(ret);
    //printf("El hash md5 de %s es %s\n", aux->filename, aux->hashmd5);
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
    //printf("Error fork!\n");
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
