#include <iostream>
#include <stdio.h>
#include <pthread.h>
int is_error;

int mutex=0;

int amount=91; //threads amount
int iter=7777; //iterations amount

int to=1;
int from=0;
int exchanging(int *point, int to, int from)
{
	int temp;
	asm volatile("pusha");
	asm volatile
	(
		"lock cmpxchg %1,%3"
		: "=a" (temp)
		: "r" (to), "0" (from), "m" (*point)
		: "memory", "cc"
	);
	asm volatile("popa");
	return temp;
}

int count = 0;
void* with_xchng(void* argument)
{
	int i=0;
	while(i<iter)
	{
		if (!exchanging(&mutex,to,from))
		{
			i++;
			count++;
			mutex=0;
		}
	}
}

int main()
{
	int i;
	
	printf("91 threads and 7777 iteration. Why this numbers?\nBecause 91*7777 = 707707 - nice number - thats all:)\n");
	printf("As you can see with exchange this number is always 707707:\nthat means what all iterations were completed.\n");
	printf("You can change numbers, but result always will be \nequal to result of their multiplication.\n");
	printf("Check it out below.\n");
	
    pthread_t thread[amount];
	
	for(i=0; i<amount; i++)
	{
		is_error = pthread_create(&thread[i], NULL, with_xchng, NULL);
		if(is_error)
		{
			printf("Can't create a thread number %d. Error %d.", i, is_error);
			return 0;
		}
	}
	for(i=0; i<amount; i++)
	{
		is_error = pthread_join(thread[i], NULL);
		if(is_error)
		{
			printf("Can't join thread number %d:. Error %d.", i, is_error);
			return 0;
		}
	} 
	
	printf("For %d threads and %d iterations result is %d\n", amount, iter, count);
	return 0;
}