#include "pthreadpool.h"

#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int pthreadpool_init(Pthreadpool_t *ptPool, int iMax_threads)
{
	ptPool->m_iMax_pthreads = iMax_threads;
	ptPool->m_iCounter = 0;
	ptPool->m_iIdle = 0;
	ptPool->m_bQuitFlag = 0;
	ptPool->m_ptFirst_task = NULL;
	ptPool->m_ptLast_task = NULL;

	if(0 != condition_init(&ptPool->m_tReady))
	{
		return -1;
	} 	
	
	return 0;
}

void* pthread_routine(void* args)
{
	struct timespec tAbstime;
	struct timeval now;
	int status = 0;
	int timeout = 0;

	Pthreadpool_t* pool = (Pthreadpool_t*)args;
	condition_lock(&pool->m_tReady);
	
	printf("pthread %d create.\n", (int)pthread_self());

	while(1)
	{
		pool->m_iIdle++;

		while(pool->m_ptFirst_task == NULL && !pool->m_bQuitFlag)	
		{				
			gettimeofday(&now, NULL);
			tAbstime.tv_sec = now.tv_sec + 2;
			tAbstime.tv_nsec = now.tv_usec*1000; 
			status = condition_timewait(&pool->m_tReady, &tAbstime);
			printf("status:%x, ETIMEDOUT:%x\n",status, (int)ETIMEDOUT);			
			if(status != 0)
			{	
				printf("pthread:%d timeout.\n", (int)pthread_self());
				timeout = 1;			
				break;			
			}								
		}

		pool->m_iIdle--;

		if(pool->m_ptFirst_task != NULL)
		{			
			Task_t *new_task = pool->m_ptFirst_task;
			pool->m_ptFirst_task = new_task->m_ptNext;
		
			condition_unlock(&pool->m_tReady);
			printf("pthread:%d\t ", (int)pthread_self());

			new_task->run(new_task->m_pArg);				
			free(new_task);
			condition_lock(&pool->m_tReady);
		}			

		if(pool->m_bQuitFlag && pool->m_ptFirst_task == NULL)
		{
			break;		
		}	

		if(timeout)
			break;						
	}
	
	condition_signal(&pool->m_tReady);

	pool->m_iCounter--;
	condition_unlock(&pool->m_tReady);
	
	printf("thread %d exit.\n", (int)pthread_self());	
}

int pthreadpool_add_task(Pthreadpool_t *ptPool, void* (*run)(void* args), void* args)
{
	Task_t* pt_new_task = (Task_t*)malloc(sizeof(Task_t));
	
	pt_new_task->run = run;
	pt_new_task->m_pArg = args;
	pt_new_task->m_ptNext = NULL;
	
	condition_lock(&ptPool->m_tReady);

	if(ptPool->m_ptFirst_task == NULL)
	{
		ptPool->m_ptFirst_task = pt_new_task;
	}	
	else
	{	
		ptPool->m_ptLast_task->m_ptNext = pt_new_task;	
	}
	
	ptPool->m_ptLast_task = pt_new_task;

	if(ptPool->m_iIdle > 0)
	{
		condition_signal(&ptPool->m_tReady);
	}
	else if(ptPool->m_iCounter < ptPool->m_iMax_pthreads)
	{
		pthread_t tid;
		pthread_create(&tid, NULL, pthread_routine, ptPool);
		ptPool->m_iCounter++;			
	}

	condition_unlock(&ptPool->m_tReady);

	return 0;
}

int pthreadpool_destroy(Pthreadpool_t *ptPool)
{
	condition_lock(&ptPool->m_tReady);

	if(ptPool->m_bQuitFlag)
	{
		condition_unlock(&ptPool->m_tReady);
		return 0;	
	}
	
//	ptPool->m_bQuitFlag = 1;

	if(ptPool->m_iCounter)
	{
		if(ptPool->m_iIdle)
		{
			condition_broadcast(&ptPool->m_tReady);		
		}
		
		while(ptPool->m_iCounter)
		{
			condition_wait(&ptPool->m_tReady);
		}
	}
	
	condition_unlock(&ptPool->m_tReady);

	return 0;
}




