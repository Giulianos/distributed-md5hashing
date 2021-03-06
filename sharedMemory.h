#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

void modifySemaphore(int x,int id_sem);

#ifndef __APPLE__
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
#endif

#endif
