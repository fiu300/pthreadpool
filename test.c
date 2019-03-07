#include "pthreadpool.h"
#include <stdio.h>
#include <stdlib.h>

void* routine(void* arg)
{	
	printf("\t\tnum: %d\n", *(int*)arg);
	free(arg);
	
	return NULL;
}

int main()
{
	Pthreadpool_t tPool;
	
	pthreadpool_init(&tPool, 3);

	for(int i=0; i<10; i++)
	{
		int* arg = malloc(sizeof(int));
		*arg = i;
		pthreadpool_add_task(&tPool, routine, arg);	
	}
	
	pthreadpool_destroy(&tPool);

	return 0;
}
