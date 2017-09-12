#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "task.h"
#include "slaveProcess.h"

void processTask(task_t * task);

int main(int argc, char * argv[])
{
  char msg[300];
	char * auxBuffer;
  char * curr = msg;
	char running = 1;
	task_t auxTask;

	auxTask.filename = (char *)calloc(256, sizeof(char));

	auxBuffer = (char *)calloc(15+MAX_FILENAME_LEN + 1 + HASHSIZE + 2, sizeof(char));

	while(running) {
		while((*curr=getchar()) && *curr!='\0') { curr++; }
		switch(msg[0]) {
			case 3: running = 0; /*fprintf(stderr, "Exit\n");*/ break;
			case 2: strncpy(auxTask.filename, msg+1, 256);
							processTask(&auxTask);
							sprintf(auxBuffer, "%s:%s \n", auxTask.filename, auxTask.hashmd5);
							write(STDOUT_FILENO, auxBuffer, strlen(auxBuffer));
		}
		curr = msg;
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
		execlp("md5sum", "md5sum" ,task->filename, NULL);
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
