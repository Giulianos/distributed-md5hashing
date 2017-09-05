#include "applicationProcess.h"
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
worker_t * createWorkers(int quantity);
void pollWorkers(worker_t * workers, int quantity);
int readFromWorker(const worker_t * worker, char * buffer, int len);
int writeToWorker(worker_t * worker, const char * msg, int len);
void assignTaskToWorker(const worker_t * worker, const task_t * task);
void dispatchTasks(const worker_t * workers, int quantity, taskQueue_t * unassignedTasks);
void stopWorkers(const worker_t * workers, int quantity);
void fetchTasks(taskQueue_t queue, int argc, char * argv[]);


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


	//Create workers
  workers = createWorkers(WORKERS_QUANTITY);

	//Create queues
	unassignedTasks = newTaskQueue();
	processedTasks = newTaskQueue();

	//Fetch tasks
	fetchTasks(unassignedTasks, argc, argv);

	//Distribute tasks
	while(!isEmpty(unassignedTasks)) {
		//Dispatch tasks
		dispatchTasks(workers, WORKERS_QUANTITY, unassignedTasks);
		//Retreive processed tasks
		pollWorkers(workers, WORKERS_QUANTITY);
	}

	stopWorkers(workers, WORKERS_QUANTITY);

  wait(NULL);
	_exit(0);

}

void stopWorkers(const worker_t * workers, int quantity)
{
	int i;
	char auxCmd = STOP_WORKER_CMD;

	for(i=0; i<quantity; i++) {
		writeToWorker(workers[i].write_pipe, &auxCmd, 1);
	}

}

void fetchTasks(taskQueue_t queue, int argc, char * argv[])
{
	int i;
	task_t auxTask;

	for(int i=1; i<argc; i++) {
		auxTask.filename = argv[i];
		auxTask.processed = 0;
		offer(queue, auxTask);
	}

}

void assignTaskToWorker(const worker_t * worker, const task_t * task)
{
	char auxCmd = ASSIGN_TASK_CMD;

	writeToWorker(worker->write_pipe, &auxCmd, 1);
	writeToWorker(worker->write_pipe, task->filename, strlen(task->filename)+1);
}

void dispatchTasks(const worker_t * workers, int quantity, taskQueue_t * unassignedTasks) {
	//UNIMPLEMENTED!!
}

void pollWorkers(worker_t * workers, int quantity)//paul walker
{/* UNIMPLEMENTED!!
  int i;
  char * auxBuffer;

  #ifdef DEBUG_MSG
  printf("Polling workers...\n");
  #endif

  auxBuffer = (char *)calloc(50, sizeof(char));
  for(i=0; i<quantity; i++) {
    #ifdef DEBUG_MSG
    printf("Writing to worker %d...\n", i);
    #endif
    writeToWorker(&(workers[i]), "hola worker", strlen("hola worker")+1);
  }

  #ifdef DEBUG_MSG
  printf("Write to all workers done!\n");
  #endif

  for(i=0; i<quantity; i++) {
    readFromWorker(&workers[i], auxBuffer, 50);
    printf("Worker %d said:\n %.50s\n", workers[i].id, auxBuffer);
  }*/

}

int readFromWorker(const worker_t * worker, char * buffer, int len)
{
  return read(worker->read_pipe, buffer, len);
}

int writeToWorker(worker_t * worker, const char * msg, int len)
{
  #ifdef DEBUG_MSG
  printf(" worker id: %d\n read_pipe: %d\n write_pipe: %d\n", worker->id, worker->read_pipe, worker->write_pipe);
  #endif
  return write(worker->write_pipe, msg, len);
}

worker_t * createWorkers(int quantity)
{
  worker_t * workers;
  int auxpipe_read[2];
  int auxpipe_write[2];
  int i, auxpid;

  #ifdef DEBUG_MSG
  printf("Creating workers...\n");
  #endif

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
  #ifdef DEBUG_MSG
  printf("Workers created!\n");
  #endif

  return workers;

}

























