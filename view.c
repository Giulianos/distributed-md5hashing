#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
 #include <sys/wait.h>
int main(int argc, char* argv[]){
	//create share memory
	key_t key;
	int id_mem;
	int *memory;
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
	printf("termino");
	return 0;

}

