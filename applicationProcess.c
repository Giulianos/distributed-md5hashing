#include "applicationProcess.h"
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <fcntl.h>

worker_t * createWorkers(int quantity);
void pollWorkers(worker_t * workers, int quantity, taskQueue_t queue, int * processedTasks);
int readFromWorker(const worker_t * worker, char * buffer, int len);
int writeToWorker(const worker_t * worker, const char * msg, int len);
void assignTaskToWorker(worker_t * worker, const task_t * task);
void dispatchTasks(worker_t * workers, int quantity, taskQueue_t unassignedTasks);
void stopWorkers(const worker_t * workers, int quantity);
int readLineFromWorker(const worker_t * worker, char * buffer, int maxlen);
int fetchTasks(taskQueue_t queue, int argc, char * argv[]);


int main(int argc, char * argv[])
{
	//create share memory
	key_t key;
	int id_mem;
	int *memory;
	key = ftok ("/$HOME", 77);
	if (key == -1){
		printf("[ERROR 01] Can't generate a share memory space\n");
		wait(NULL);
		exit(0);
	}
	id_mem = shmget (key, sizeof(int)*1000000, 0777 | IPC_CREAT);
	if (id_mem == -1){
		printf("[ERROR 02] Can't generate a share memory space\n");
		wait(NULL);
		exit (0);
	}
	memory = (int *)shmat (id_mem, (char *)0, 0);
	if (memory == NULL){
		printf("[ERROR 03] Can't generate a share memory space\n");
		wait(NULL);
		exit (0);
	}

  worker_t * workers;
	taskQueue_t unassignedTasks;
	taskQueue_t processedTasks;
	int fetchedTasks;
	int finishedTasks;


	//Create workers
	printf("Creating workers...");
  workers = createWorkers(WORKERS_QUANTITY);
	printf("\x1B[32m[DONE!]\x1B[0m\n");

	//Create queues
	printf("Creating queues...");
	unassignedTasks = newTaskQueue();
	processedTasks = newTaskQueue();
	printf("\x1B[32m[DONE!]\x1B[0m\n");

	//Fetch tasks
	printf("Fetching tasks...");
	finishedTasks = 0;
	fetchedTasks = fetchTasks(unassignedTasks, argc, argv);
	printf("\x1B[32m[DONE!]\x1B[0m\n");
	printf("Fetched %d tasks!\n", fetchedTasks);

	//Distribute tasks
	printf("Distributing tasks...\n");
	while(!isEmpty(unassignedTasks)) {
		//Dispatch tasks
		dispatchTasks(workers, WORKERS_QUANTITY, unassignedTasks);
		//Retreive processed tasks
		pollWorkers(workers, WORKERS_QUANTITY, processedTasks, &finishedTasks);
	}
	printf("All tasks have been assigned!\n");
	printf("Waiting %d tasks to finish...\n", fetchedTasks-finishedTasks);
	while(finishedTasks!=fetchedTasks) {
		pollWorkers(workers, WORKERS_QUANTITY, processedTasks, &finishedTasks);
	}
	printf("Stopping workers...");
	stopWorkers(workers, WORKERS_QUANTITY);
	printf("\x1B[32m[DONE!]\x1B[0m\n");

	printf("Exiting...\n");

  wait(NULL);
	_exit(0);

}


void stopWorkers(const worker_t * workers, int quantity)
{
	int i;
	char auxCmd;

	for(i=0; i<quantity; i++) {
		auxCmd = STOP_WORKER_CMD;
		writeToWorker(&workers[i], &auxCmd, 1);
		auxCmd = '\0';
		writeToWorker(&workers[i], &auxCmd, 1);
	}

}

int fetchTasks(taskQueue_t queue, int argc, char * argv[])
{
	int i;
	task_t auxTask;

	for(i=1; i<argc; i++) {
		auxTask.filename = argv[i];
		auxTask.processed = 0;
		offer(queue, auxTask);
	}
	return argc-1;
}

void assignTaskToWorker(worker_t * worker, const task_t * task)
{
	char auxCmd = ASSIGN_TASK_CMD;

	#ifdef DEBUG_MSG
  printf("\x1B[33mAssigned\x1B[0m %s to worker %d\n", task->filename, worker->id);
  #endif
	writeToWorker(worker, &auxCmd, 1);
	writeToWorker(worker, task->filename, strlen(task->filename)+1);
}

void dispatchTasks(worker_t * workers, int quantity, taskQueue_t unassignedTasks) {
	int workerNum, jobNum;
	char adding = 0;

	for(workerNum=0; workerNum<quantity && !isEmpty(unassignedTasks); workerNum++) {
		for(jobNum=0; jobNum<JOBSIZE && !isEmpty(unassignedTasks); jobNum++) {
			if(workers[workerNum].unprocessed == 0 || adding) {
					assignTaskToWorker(&workers[workerNum], poll(unassignedTasks));
					workers[workerNum].unprocessed++;
					adding = 1;
			}
		}
		adding = 0;
	}
}

void pollWorkers(worker_t * workers, int quantity, taskQueue_t queue, int * processedTasks)
{
	int i;
	int read, cant=0;
	char * auxBuffer;

	auxBuffer = calloc(300, sizeof(char));
	for(i=0; i<quantity; i++) {
		while(readLineFromWorker(&workers[i], auxBuffer, 300)) {
			printf("\x1B[32mFinished\x1B[0m %s\n", auxBuffer);
			workers[i].unprocessed--;
			(*processedTasks)++;
		}
	}
}

int readFromWorker(const worker_t * worker, char * buffer, int len)
{
  return read(worker->read_pipe, buffer, len);
}

int readLineFromWorker(const worker_t * worker, char * buffer, int maxlen)
{
	int status = 1;

	while((status=readFromWorker(worker, buffer, 1))!=-1 && *(buffer)!='\n' && maxlen>0) {
		//printf("Read %c from worker %d\n", *buffer, worker->id);
		maxlen--;
		buffer++;
	}
	//Let's replace \n with \0
	*(buffer-1) = '\0';
	return status==-1 ? 0 : 1;
}

int writeToWorker(const worker_t * worker, const char * msg, int len)
{
	//printf("Wrote %s to worker %d\n", msg, worker->id);
  return write(worker->write_pipe, msg, len);
}

worker_t * createWorkers(int quantity)
{
  worker_t * workers;
  int auxpipe_read[2];
  int auxpipe_write[2];
  int i, auxpid;

  workers = (worker_t *)calloc(quantity, sizeof(worker_t));

  if(workers == NULL) {
    perror("[ERROR!] Couldn't allocate space for workers!");
    wait(NULL);
    _exit(1);
  }

  for(i=0; i < quantity; i++) {
    pipe(auxpipe_write);
    pipe(auxpipe_read);
    workers[i].id = i;
    workers[i].unprocessed = 0;
    workers[i].write_pipe = auxpipe_write[1];
    workers[i].read_pipe = auxpipe_read[0];
		fcntl(workers[i].read_pipe, F_SETFL, O_NONBLOCK);
    auxpid = fork();
    switch(auxpid) {
      case -1:
        perror("[ERROR!] Couldn't fork worker!");
        wait(NULL);
        _exit(1);
      case 0: //Child
        close(STDIN_FILENO);
        dup(auxpipe_write[0]);
        close(STDOUT_FILENO);
        dup(auxpipe_read[1]);
        close(auxpipe_read[1]);
        close(auxpipe_read[0]);
        close(auxpipe_write[1]);
        close(auxpipe_write[0]);
        execlp(SLAVE_EXEC_NAME, SLAVE_EXEC_NAME, NULL);
        perror("[ERROR!] Couldn't execute worker in forked child!");
        wait(NULL);
        _exit(1);
      default:
        workers[i].pid = auxpid;
        close(auxpipe_write[0]);
        close(auxpipe_read[1]);
    }
  }

  return workers;

}

























