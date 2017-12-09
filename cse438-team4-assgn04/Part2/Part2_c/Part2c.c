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
pthread_t threadID;
pthread_t dummyID;
int mouseFlag = 0;
struct timespec time1, time2;

void* Daemon_Threadfunc(void *ptr);
void Signal_Handler(int signo);
void* Dummy_Threadfunc(void *ptr);

int main()
{
	struct sigaction sa;
	
	/* set up the signal handler */
	memset(&sa, 0, sizeof (sa));
	sa.sa_handler = &Signal_Handler;

	memset((void *)&time1, 0, sizeof(time1));
	memset((void *)&time2, 0, sizeof(time1));

	/* Create two threads
	 * Thread 1 : Daemon thread to detect mouse event 
	 * Thread 2 : Dummy thread which is executing nanosleep */
	pthread_create(&threadID, NULL, &Daemon_Threadfunc, NULL);
	pthread_setname_np(threadID,"Mouse_Thread");
	pthread_create(&dummyID, NULL, &Dummy_Threadfunc, NULL);
	pthread_setname_np(dummyID,"Thread_Nanosleep");

	sigaction(SIGUSR1, &sa, NULL);

	/* When mouse event detected, raise signal */
	while(mouseFlag==0)
		{};
	pthread_kill(dummyID,SIGUSR1);

	pthread_join(threadID,NULL); 
	pthread_join(dummyID,NULL);

return 0;
}

/* Thread to implement signal handling during nanosleep */
void* Dummy_Threadfunc(void *ptr)
{
	int ret;
	time1.tv_sec = 10;
	time1.tv_nsec = 0;

	printf("In the Dummy thread\n");
	printf("Waiting for nanosleep to expire in 10 secs\n");

	ret = nanosleep(&time1,&time2);
	
	if(ret < 0)
	{/* When interrupted by signal, -1 is returned */
	printf("Signal interrupt. Time remaining is %LF sec\n", (long double)time2.tv_sec + (long double)time2.tv_nsec /1000000000);
	}
	else
		printf("No signal interrupt. Delay success. Right click to exit \n");

	return NULL;
}

/* Signal handler for SIGUSR1*/
void Signal_Handler(int signo)
{
	printf("In the signal handler\n");	
}

/* To detect right click evevnt and set flag */
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
				mouseFlag=1;
				break;
			}
		}
	}
	printf("Exiting Daemon thread\n");
	return NULL;
}

