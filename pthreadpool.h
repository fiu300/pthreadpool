#ifndef _PTHREADPOOL_H_
#define _PTHREADPOOL_H_

#include "condition.h"

typedef struct Task{
	void* (*run)(void* args);
	void* m_pArg;
	struct Task* m_ptNext;
}Task_t;

typedef struct Pthreadpool{
	Condition_t m_tReady;
	int m_iMax_pthreads;
	int m_iCounter;
	int m_iIdle;
	int m_bQuitFlag;
	Task_t* m_ptFirst_task;
	Task_t* m_ptLast_task;
}Pthreadpool_t;

int pthreadpool_init(Pthreadpool_t *ptPool, int iMax_threads);
int pthreadpool_add_task(Pthreadpool_t *ptPool, void* (*run)(void* args), void* args);
int pthreadpool_destroy(Pthreadpool_t *ptPool);

#endif
