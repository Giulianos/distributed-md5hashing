#include "view.h"

int main(int argc, char* argv[]){
	//create share memory
	key_t key;
	int id_mem;
	int *memory;
	int id_sem;
	int current=1;
	union semun arg;
	int currentmem=1;
	int state=0;


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

	
	//printf("llego\n" );
	int x=2;
	while(memory[0]!= EOF || memory[0] > x){
		
		modifysemaphore(-1,id_sem);
		//printf("%d %d\n",memory[0],x);
		if(memory[0]>x){
			//printf("entra2\n");
			while(x<memory[0]){
				printf("%c",memory[x]);
				x++;
			}
		}
		if(memory[0]>10000-300){	
			memory[0]=2;
			x=2;
		}
		modifysemaphore(1,id_sem);
		//printf("sale sem\n");
	}
	//	printf("\n");
	  modifysemaphore(-1,id_sem);
	  memory[1]=-1;
	  modifysemaphore(1,id_sem);
	 //printf("sale\n");


	return 0;

}

void modifysemaphore(int x,int id_sem){
	struct sembuf operation;
	operation.sem_num = 0;
	operation.sem_op = x;
	operation.sem_flg = 0;
	semop (id_sem, &operation, 1);
}