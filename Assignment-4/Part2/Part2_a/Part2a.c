/* Course : CSE 438 - Embedded Systems Programming
 * Assignment 4 : Event Handling and Signaling in Linux
 * Team Member1 : Samruddhi Joshi  ASUID : 1213364722
 * Team Member2 : Varun Joshi 	   ASUID : 1212953337 */

#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/input.h> /* required for mouse events*/
#include <math.h>
#include <pthread.h>     /* required for pthreads */
#include <semaphore.h>   /* required for semaphores */
#include <sched.h> 	 /* required for setting affinity */
#include <unistd.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/poll.h>
#include <sys/syscall.h>

void Signal_Handler(int signo);
void* Thread_HighPrio(void *ptr);
void* Thread_LowPrio(void *ptr);

/* Global Variables Declaration */
pthread_t thread_ID[2];    /* 1 SPI_transmit thread, 1 sensor_detect */
pthread_attr_t thread_attr[2];
int thread_priority[]={6,1};  
struct sched_param param[2];
int rerror[2]; /* to check for error in pthread creation */

void error(char* msg)
{
	printf("%s\n", msg);
	exit(EXIT_SUCCESS);
}

int main(void)
{
	int i,test;
	struct sigaction sa;
	//struct timespec ts;

	sa.sa_handler = Signal_Handler;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	if(sigaction(SIGUSR1, &sa, NULL) == -1)
		error("sigaction");
	

	/* Create two pthreads */
	for(i=0; i<2; i++)
	{
		pthread_attr_init(&thread_attr[i]);
		pthread_attr_getschedparam(&thread_attr[i], &param[i]);
		param[i].sched_priority = thread_priority[i];  /* set thread priority */
		pthread_attr_setschedparam(&thread_attr[i], &param[i]);
		pthread_setschedparam(pthread_self(), SCHED_RR, &param[i]);
	}
	rerror[1] = pthread_create(&thread_ID[1], &thread_attr[1], &Thread_LowPrio, NULL);
	pthread_setname_np(thread_ID[1],"Thread_LowPrio");
	
	rerror[0] = pthread_create(&thread_ID[0], &thread_attr[0], &Thread_HighPrio, NULL);	
	pthread_setname_np(thread_ID[0],"Thread_HighPrio");
	

	if((rerror[0] != 0) || (rerror[1] != 0))
	{
		printf("Error while creating thread \n");
	}
	
	for(i=0; i<2; i++)
	{/* wait for all threads to terminate */
		pthread_join(thread_ID[i],NULL); 
	}
	
return 0;
}

void* Thread_HighPrio(void *ptr)
{
	int i,j,k;
	pthread_kill(thread_ID[1],SIGUSR1);
	printf("In Thread High Prio\n");
	for(i=0;i<500;i++)
	{
		for(j=0;j<1000;j++)
		{
		k++;
		}
	}
	printf("HP thread id %ld\n", syscall(SYS_gettid));
	
	pthread_exit(0);
}
int flag =0;

void* Thread_LowPrio(void *ptr)
{
	printf("In Thread Low Prio\n");
	sleep(2);
	while(!flag)
		{};
	printf("LP thread id %ld\n", syscall(SYS_gettid));
	
	pthread_exit(0);
}

/*
 * Signal handler for exception */
void Signal_Handler(int signo)
{
test=10;
printf("%d\n",test);	
printf("In Signal Handler \n");
	flag = 1;
}
