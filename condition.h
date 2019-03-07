#ifndef _CONDITION_H_
#define _CONDITION_H_
#include <pthread.h>

typedef struct Condition
{
	pthread_mutex_t m_mtx;
	pthread_cond_t m_con;
}Condition_t;

int condition_init(Condition_t *ptCond);
int condition_lock(Condition_t *ptCond);
int condition_unlock(Condition_t *ptCond);
int condition_signal(Condition_t *ptCond);
int condition_broadcast(Condition_t *ptCond);
int condition_wait(Condition_t *ptCond);
int condition_timewait(Condition_t *ptCond, const struct timespec* ptAbstime);
int condition_destroy(Condition_t *ptCond);


#endif
