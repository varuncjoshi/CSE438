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
#include <string.h>


#define MOUSEFILE "/dev/input/event7"
#define MOUSE 0x01 		//EV_KEY
#define RIGHT_CLICK 0x111 	//BUTTON_RIGHT
#define KEY_PRESS 0x1

sem_t semLock;
pthread_t Thread_ID;
pthread_t Dummy_ID;
int mouseFlag = 0;

void* Daemon_Threadfunc(void *ptr);
void Signal_Handler(int signo);
void* DummyThread(void *ptr);

int main()
{

	struct sigaction sa;
	memset(&sa, 0, sizeof (sa));
	sa.sa_handler = &Signal_Handler;

	/* Initialise the semaphore to 0 value */
	sem_init(&semLock,0,0);

	/* Create two threads
	 * Thread 1 : Daemon thread to detect mouse event 
	 * Thread 2 : Dummy thread which is waiting for semaphore */
	pthread_create(&Thread_ID, NULL, &Daemon_Threadfunc, NULL);
	pthread_setname_np(Thread_ID,"Mouse_Thread");
	pthread_create(&Dummy_ID, NULL, &DummyThread, NULL);
	pthread_setname_np(Dummy_ID,"Thread_SemWait");

	sigaction(SIGUSR1, &sa, NULL);

	/* When mouse event detected, raise signal */
	while(mouseFlag==0)
			{};
	pthread_kill(Dummy_ID,SIGUSR1);

	pthread_join(Thread_ID,NULL); 
	pthread_join(Dummy_ID,NULL);

return 0;
}

/* Thread to implement signal handling during sem_wait */
void* DummyThread(void *ptr)
{
	printf("Waiting for SemPost\n");
	printf("Right Click to generate signal\n");

	sem_wait(&semLock);

	printf("sem_wait is released because of return from signal handler\n");
	printf("signal handler usage should be done carefully\n");

	return NULL;
}

/*Signal handler for SIGUSR1*/
void Signal_Handler(int signo)
{
	printf("In the Signal Handler\n");	
}

/*To detect right click event and post semaphore*/
void* Daemon_Threadfunc(void *ptr)
{

	int filedescriptor; 
	struct input_event click;
	
	if((filedescriptor = open(MOUSEFILE, O_RDONLY)) == -1)
	{
		perror("opening device");
		exit(EXIT_FAILURE);
	}

	while(read(filedescriptor, &click, sizeof(struct input_event)))
	{
		if(click.type == MOUSE)  
		{
			if((click.code == RIGHT_CLICK) && (click.value == KEY_PRESS))
			{
				/* Right click event occured */
				mouseFlag = 1;
				break;
			}
		}
	}
	printf("Exiting Daemon thread\n");
	return NULL;
}

