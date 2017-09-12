#include "sharedMemory.h"

void modifySemaphore(int x,int id_sem){
	struct sembuf operation;
	operation.sem_num = 0;
	operation.sem_op = x;
	operation.sem_flg = 0;
	semop(id_sem, &operation, 1);
}
