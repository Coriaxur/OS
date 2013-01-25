#include <stdio.h>
#include <ucontext.h>
#include <sys/time.h>
#include <stdlib.h>
#include <poll.h>

#define NumberOfContexts 16
#define StackSize 4096
int MyInterval = 100;
int ThreadTicks = 15;
int CurrentContextId = 0;
int Thread1SleepTime = 10;
int Thread2SleepTime = 8;

struct context_options
{
	ucontext_t Context;
	ucontext_t ExitContext;
	int ContextId;
	int ContextStatus;
	int ContextSleepTime;
	char Stack[StackSize];
	char ExitStack[StackSize];
} MyTreads[NumberOfContexts];

struct itimerval AlarmInterval;

ucontext_t SchedulerContext;

void *signal_stack;

void thread_exit()
{
	MyTreads[CurrentContextId].ContextStatus = 0;
}

int thread_create(void(*fucnt)())
{
	int id;
	for(id = 1; id < NumberOfContexts; id++)
	{
		if(MyTreads[id].ContextStatus == 0)
		{
			MyTreads[id].ContextId = id;
			MyTreads[id].ContextStatus = 1;
			MyTreads[id].ContextSleepTime = 0;
			break;
		}
	}

	getcontext(&MyTreads[id].ExitContext);
	MyTreads[id].ExitContext.uc_stack.ss_sp = MyTreads[id].ExitStack;
	MyTreads[id].ExitContext.uc_stack.ss_size = sizeof(MyTreads[id].ExitStack);
	MyTreads[id].ExitContext.uc_link = &SchedulerContext;
	makecontext(&MyTreads[id].ExitContext, thread_exit, 0);


	getcontext(&MyTreads[id].Context);
	MyTreads[id].Context.uc_stack.ss_sp = MyTreads[id].Stack;
	MyTreads[id].Context.uc_stack.ss_size = sizeof(MyTreads[id].Stack);
	MyTreads[id].Context.uc_link = &MyTreads[id].ExitContext;
	makecontext(&MyTreads[id].Context, fucnt, 0);

	return id;
}

void thread_sleep(int counts)
{
	MyTreads[CurrentContextId].ContextStatus = 2;
	MyTreads[CurrentContextId].ContextSleepTime = counts;
	int TempContextId = CurrentContextId;
	CurrentContextId = 0;
	swapcontext(&MyTreads[TempContextId].Context, &SchedulerContext);
}

void thread_1()
{
	int i;
	for(i=0; i<ThreadTicks; i++)
	{
		printf("-Thread 1 working...\n");
		thread_sleep(Thread1SleepTime);
	}
}

void thread_2()
{
	int i;
	for (i=0; i<ThreadTicks; i++)
	{
		printf("--Thread 2 working..\n");
		thread_sleep(Thread2SleepTime);
	}
}


void thread_wait(int tid)
{
	int l = (MyInterval*1000)%1000000;
	while (MyTreads[tid].ContextStatus != 0) 
	{
		poll(NULL,0,l);
	}
}

void swap_threads(int tid)
{
	if (tid != 0)
	{
		CurrentContextId = tid;
		swapcontext(&SchedulerContext, &MyTreads[CurrentContextId].Context);
	}
}

void schedul_function()
{
	int NextId = 0;
	int CurrentId = 0;
	while(CurrentId < NumberOfContexts)
	{
		if (MyTreads[CurrentId].ContextStatus == 2)
		{//Sleeping?
			MyTreads[CurrentId].ContextSleepTime -= 1;
			if (MyTreads[CurrentId].ContextSleepTime == 0)
			{
				MyTreads[CurrentId].ContextStatus = 1;//Active!
			}
		}
		CurrentId++;
	}

	//Moving...
	CurrentId = 0;
	while(CurrentId < NumberOfContexts)
	{
		if ((CurrentContextId!=CurrentId)&&(MyTreads[CurrentId].ContextStatus==1))
		{
			NextId = CurrentId;
			break;
		}
		CurrentId++;
	}
	swap_threads(NextId);
}	

void scheduler_init(ucontext_t * init_context)
{
	getcontext(init_context);
	init_context->uc_stack.ss_sp=signal_stack;
	init_context->uc_stack.ss_size=StackSize;
	init_context->uc_stack.ss_flags=0;
	sigemptyset(&init_context->uc_sigmask);
	makecontext(init_context,schedul_function,0);
}

int main()
{
	signal_stack = malloc (StackSize);
	if (signal_stack == NULL) 
	{
       	printf("Cant malloc\n");
       	exit(1);
    }
	int i;
	for (i=1; i<NumberOfContexts; i++)
	{
		MyTreads[i].ContextStatus = 0;//Free Tread
	}
	
	int l = (MyInterval*1000)%1000000;
	AlarmInterval.it_interval.tv_sec = 0;
	AlarmInterval.it_interval.tv_usec = l;
	AlarmInterval.it_value.tv_sec = 0;
	AlarmInterval.it_value.tv_usec = l;
	if (setitimer(ITIMER_REAL, &AlarmInterval, 0)<0)
	{
		printf("Timer cant be set\n");
		exit(1);
	};
	
	signal(SIGALRM, schedul_function); 
	scheduler_init(&SchedulerContext);
	int Thread1 = thread_create(thread_1);
	int Thread2 = thread_create(thread_2);
	thread_wait(Thread1);
	thread_wait(Thread2);
	
	return 0;
}
