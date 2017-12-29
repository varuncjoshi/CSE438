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

/* Global Variables Declaration */
struct timespec time1, time2;
pthread_t threadID1, threadID2, threadID3, threadID4;
struct sigaction sa; /*declare signal action structure globally*/
int mouseFlag;
sigset_t mask2;

/* Local declarations */
void thread1();
void thread2();
void thread3();
void* Daemon_Threadfunc(void *ptr);
void Signal_Handler(int signo);

void error(char* msg)
{
	printf("%s\n", msg);
	exit(EXIT_SUCCESS);
}

void thread1()
{
	/* Wait till the signal delivery to invoke signal handler */
	printf("Thread 1 waiting for SIGIO signal \n");
	sigsuspend(&mask2);
	while(1)
	{
		printf("Thread 1 active\n");
		usleep(100000);
	}
}

void thread2()
{
	/* Wait till the signal delivery to invoke signal handler */
	printf("Thread 2 waiting for SIGIO signal \n");
	sigsuspend(&mask2);
	while(1)
	{
		printf("Thread 2 active\n");
		usleep(100000);
	}
}

void thread3()
{
	/* Wait till the signal delivery to invoke signal handler */
	printf("Thread 3 waiting for SIGIO signal \n");
	sigsuspend(&mask2);
	while(1)
	{
		printf("Thread 3 active\n");
		usleep(100000);
	}
}

/* Signal handler function for SIGIO - executed when signal is recieved */
void Signal_Handler(int signo)
{
	printf("Inside signal handler\n");
}

/*Daemon thread that continuously checks for right click event*/
void* Daemon_Threadfunc(void *ptr)
{

	int filedescriptor; 
	struct input_event click;
	
	if((filedescriptor = open(MOUSEFILE, O_RDONLY)) == -1)
	{/* Open the mouse device file */
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
			}
		}

		if(sigaction(SIGUSR1, &sa, NULL) == -1)
				error("sigaction");
	}
	printf("Exiting Daemon thread\n");
	return NULL;
}


int main(int argc,char* argv[])
{
	int k;	
	sigset_t set;
	
	/* Set up the signal Handler */
	sa.sa_handler = Signal_Handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	if(sigaction(SIGIO, &sa, NULL) == -1)
		error("sigaction");
	/* Fill the signal set */
	sigfillset(&mask2);
	/* Delete SIGUSR1 from signal set*/
	sigdelset(&mask2,SIGUSR1);

	sigemptyset(&set);
	sigaddset(&set,SIGUSR1);

	/* Mask signal for the main thread */
	if(pthread_sigmask(SIG_BLOCK,&set,NULL))
	{
		error("pthread sigmask");
	}

	/* Create threads */
	pthread_create(&threadID1, NULL,(void *)&thread1,NULL );
	pthread_setname_np(threadID1,"Thread1");
	pthread_create(&threadID2, NULL,(void *)&thread2,NULL );
	pthread_setname_np(threadID2,"Thread2");
	pthread_create(&threadID3, NULL,(void *)&thread3,NULL );
	pthread_setname_np(threadID3,"Thread3");
	pthread_create(&threadID4, NULL,(void *)&Daemon_Threadfunc,NULL );
	pthread_setname_np(threadID4,"Thread4");

	printf("Press right click mouse button for sigio event \n");

	while(1)
	{
		if(mouseFlag)
		{/* In event of mouse click the signal is generated and arbitarily thread is selected */
			k++;
			mouseFlag = 0;
			kill(getpid(),SIGUSR1);
			if(k == 4)/* After 4 clicks the application exits */
				exit(EXIT_SUCCESS);
		}
	}

	exit(EXIT_FAILURE);
}


