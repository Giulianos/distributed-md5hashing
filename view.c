#include "sharedMemory.h"

int main(int argc, char* argv[]){
	//create share memory
	key_t key;
	int id_mem;
	int *memory;
	int id_sem;
	int current=1;
	union semun arg;
	int currentmem=1;
	int state=1;
	int quantity=1;
	int x=2;
	int end=1;


	key = ftok ("/home", 7);
	if (key == -1){
		printf("[ERROR 01] Can't generate a share memory space\n");
		wait(NULL);
		exit(0);
	}
	id_mem = shmget (key,sizeof(int)*10000, 0777 | IPC_CREAT);
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

	id_sem = semget (key, 1, 0600 | IPC_CREAT);
	if (id_sem == -1)
	{
		printf("[ERROR 04] Can't generate a semaphore\n");
		wait(NULL);
		exit (0);
	}

	while(end) {
		modifySemaphore(-1,id_sem);
		if(memory[0]!= EOF || memory[0] > x){
			if(memory[0]>x){
				while(x<memory[0]){
					if(state==1){
						printf("\x1B[32mFinished[%d]\x1B[0m",quantity++);
						state=0;
					}
					printf("%c",memory[x]);
					if(memory[x]=='\n')
						state=1;
					x++;
				}
			}
			if(memory[0]>2000){
				memory[0]=2;
				x=2;
			}
		}else{
			end=0;
		}
		modifySemaphore(1,id_sem);
	}
	  modifySemaphore(-1,id_sem);
	  memory[1]=-1;
	  modifySemaphore(1,id_sem);


	return 0;

}
