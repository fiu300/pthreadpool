#include "condition.h"

int condition_init(Condition_t *ptCond)
{
  int status;
  
  status = pthread_mutex_init(&ptCond->m_mtx, NULL);
  if(status)
    {
      return status;
    }
  
  status = pthread_cond_init(&ptCond->m_con, NULL);
  if(status)
    {
      return status;
    }

  return status;
}

int condition_lock(Condition_t* ptCond)
{
  	if( 0 != pthread_mutex_lock(&ptCond->m_mtx))
	{
		return -1;
	}
	
	return 0;
}

int condition_unlock(Condition_t *ptCond)
{
	if( 0 != pthread_mutex_unlock(&ptCond->m_mtx))
	{
		return -1;
	}
	
	return 0;
}

int condition_signal(Condition_t *ptCond)
{
	if( 0 != pthread_cond_signal(&ptCond->m_con))
	{
		return -1;	
	}
	
	return 0;
}

int condition_broadcast(Condition_t *ptCond)
{
	if( 0 != pthread_cond_broadcast(&ptCond->m_con))
	{
		return -1;	
	}
	
	return 0;
}

int condition_wait(Condition_t *ptCond)
{
	if( 0 != pthread_cond_wait(&ptCond->m_con, &ptCond->m_mtx))
	{
		return -1;	
	}
	
	return 0;
}

int condition_timewait(Condition_t *ptCond, const struct timespec* ptAbstime)
{
	if( 0 != pthread_cond_timedwait(&ptCond->m_con, &ptCond->m_mtx, ptAbstime))
	{
		return -1;
	}

	return 0;
}

int condition_destroy(Condition_t *ptCond)
{
	if( 0 != pthread_cond_destroy(&ptCond->m_con))
	{
		return -1;
	}
	
	if( 0 != pthread_mutex_destroy(&ptCond->m_mtx))
	{
		return -1;
	}

	return 0;
}
