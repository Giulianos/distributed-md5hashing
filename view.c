#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
 #include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
void modifysemaphore(int x,key_t id_sem);
#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
// La union ya está definida en sys/sem.h
#else
// Tenemos que definir la union
union semun 
{ 
	int val;
	struct semid_ds *buf;
	unsigned short int *array;
	struct seminfo *__buf;
};
#endif






int main(int argc, char* argv[]){
	//create share memory
	key_t key;
	int id_mem;
	int *memory;
	int id_sem;
	int current=0;
	union semun arg;




	key = ftok ("/home", 77);
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

	id_sem = semget (key, 1, 0600 | IPC_CREAT);
	if (id_sem == -1)
	{
		printf("[ERROR 04] Can't generate a semaphore\n");
		wait(NULL);
		exit (0);
	}

	arg.val = 1;
	semctl (id_sem, 0, SETVAL, &arg);
	while(memory[current]!=EOF){
		modifysemaphore(-1,id_sem);
		while(memory[current]!='\0'){
			printf("%c",memory[current]);
			current++;
		}
		current++;
		if(memory[current]!=EOF){
			current=0;
			memory[current]='\0';	
		}	
		modifysemaphore(1,id_sem);
		sleep(1);
	}



	return 0;

}

void modifysemaphore(int x,key_t id_sem){
	struct sembuf operation;
	operation.sem_num = 0;
	operation.sem_op = x;
	operation.sem_flg = 0;
	semop (id_sem, &operation, 1);
}