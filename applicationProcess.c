#include "applicationProcess.h"

int currentmem=2;
int *memory;
FILE *fp;

int main(int argc, char * argv[])
{
	//create share memoryx
	key_t key;
	int id_mem;

	int id_sem;
	int state=1;
	int auxpid;
	union semun arg;

	worker_t * workers;
	taskQueue_t unassignedTasks;
	taskQueue_t processedTasks;
	int fetchedTasks;
	int finishedTasks;


	fp = fopen("hash.txt", "w+");
	if (fp==NULL) {
		perror("File error");
		exit(1);
	}

	key = ftok("/home", getpid());
	if (key == -1) {
		perror("Can't generate a share memory space");
		wait(NULL);
		exit(1);
	}
	id_mem = shmget(key, sizeof(int)*10000, 0777 | IPC_CREAT);
	if (id_mem == -1) {
		perror("Can't generate a share memory space");
		wait(NULL);
		exit(1);
	}
	memory = (int *)shmat(id_mem, NULL, 0);
	if (memory == NULL) {
		perror("Can't generate a share memory space");
		wait(NULL);
		exit(1);
	}
	id_sem = semget (key, 1, 0600 | IPC_CREAT);
	if (id_sem == -1) {
		perror("Can't generate a semaphore set");
		wait(NULL);
		exit(1);
	}
	arg.val = 0;
	semctl(id_sem, 0, SETVAL, &arg);
	modifySemaphore(1, id_sem);
	memory[0]=0;
	memory[1]=0;

	printf("You can start view now if you want. Remember, PID=%d\n", getpid());
	printf("Starting application...[Press ENTER to continue!]\n");
	while(getchar()!='\n'){}

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
	printf("Fetching tasks...\n");
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
		pollWorkers(workers, WORKERS_QUANTITY, processedTasks, &finishedTasks,id_sem);
	}
	printf("All tasks have been assigned!\n");
	printf("Waiting %d tasks to finish...\n", fetchedTasks-finishedTasks);
	while(finishedTasks!=fetchedTasks) {
		pollWorkers(workers, WORKERS_QUANTITY, processedTasks, &finishedTasks,id_sem);
	}

	sleep(0.5);
	modifySemaphore(-1,id_sem);
	memory[0]=EOF;
	modifySemaphore(1,id_sem);
	// while(state){
	// 	modifySemaphore(-1,id_sem);
	// 	if(memory[1]==-1){
	// 	 		state=0;
	//  	}
	// 	modifySemaphore(1,id_sem);
	// }

	printf("Stopping workers...\n");
	stopWorkers(workers, WORKERS_QUANTITY);
	printf("\x1B[32m[DONE!]\x1B[0m\n");

	printf("Exiting...\n");


	fclose ( fp );
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
	task_t auxTask; int fetched = 0;

	for(i=1; i<argc; i++) {
		 if(regularFileCheck(argv[i])) {
      		auxTask.filename = argv[i];
	      	auxTask.processed = 0;
		    offer(queue, auxTask);
		    fetched++;
	    } else {
	      printf("\x1B[33mOmitted\x1B[0m %s, not a file!\n", argv[i]);
	    }
  }
   return fetched;
}


int regularFileCheck(const char * filename)
{
  struct stat sb;

  /* Code taken from $man 2 stat */
  stat(filename, &sb);
  return (sb.st_mode & S_IFMT) == S_IFREG;
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

void pollWorkers(worker_t * workers, int quantity, taskQueue_t queue, int * processedTasks,int id_sem)
{
	int i;
	int read, cant=0;
	char * auxBuffer;
	int currentBuffer=0;
	int carrie=1;
	int state=1;
	//modifySemaphore(-1,id_sem);
	//modifySemaphore(1,id_sem);
	auxBuffer = calloc(500, sizeof(char));
	for(i=0; i<quantity; i++) {
		while(readLineFromWorker(&workers[i], auxBuffer, 500)) {
			//printf("Started polling worker!\n");
			fputs(auxBuffer,fp);
			fputs("\n", fp);
			state=1;
			currentBuffer=0;
			if(currentmem>2000){
				//printf("(currentmem>2)=true\n");
				while(state){
					modifySemaphore(-1,id_sem);
					if(memory[0]!=2){
						sleep(0.5);
						state=0;
						currentmem=2;
					}else{
						state=0;
						currentmem=2;
					}
					modifySemaphore(1,id_sem);
				}
			}
			//printf("modifySemaphore...\n");
			modifySemaphore(-1,id_sem);
			//printf("done!\n");
			while(auxBuffer[currentBuffer]!='\0'){
				memory[currentmem]=auxBuffer[currentBuffer];
				currentBuffer++;
				currentmem++;
			}
			memory[currentmem]='\n';
			currentmem++;
			memory[0]=currentmem;
			modifySemaphore(1,id_sem);
			workers[i].unprocessed--;
			(*processedTasks)++;
		}

	}
	free(auxBuffer);
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
