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
#include <sched.h> 	    
#include <unistd.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/poll.h>
#include <string.h>
#include "lib.h"

#define MOUSEFILE "/dev/input/event5"
#define MOUSE 0x01 		//EV_KEY
#define RIGHT_CLICK 0x111 	//BUTTON_RIGHT
#define KEY_PRESS 0x1
/*Global variable declaration*/
pthread_t threadID[6];
pthread_t daemonID;
struct sigaction sa;
int mouseFlag;
sigset_t mask2;
/* Set 1 to register a thread 
 * 1 - Thread registered to receive signal
 * 0 - Thread not registered to receive signal */
int sigMask[] = {1,0,1,0,1,1}; 

/* Local function Declarations */
void* thread1(int* m);
void* thread2(int* m);
void* thread3(int* m);
void* thread4(int* m);
void* thread5(int* m);
void* thread6(int* m);
void Signal_Handler(int signo);

void error(char* msg)
{
	printf("%s\n", msg);
	exit(EXIT_SUCCESS);
}

void* thread1(int* m)
{/*Is Signal Mask bit is set ?*/
	if(*m == 1)
	{/* Wait for signal delivery */
		sigsuspend(&mask2);
		sendToAll(6, threadID, sigMask);
		printf("Thread 1 active\n");
	}
	return NULL;
}

void* thread2(int* m)
{/*Is Signal Mask bit is set ?*/
	if(*m == 1)
	{/* Wait for signal delivery */	
		sigsuspend(&mask2);
		sendToAll(6, threadID, sigMask);
		printf("Thread 2 active\n");
	}
	return NULL;
}

void* thread3(int* m)
{/*Is Signal Mask bit is set ?*/
	if(*m == 1)
	{/* Wait for signal delivery */	
		sigsuspend(&mask2);
		sendToAll(6, threadID, sigMask);
		printf("Thread 3 active\n");
	}
	return NULL;
}

void* thread4(int* m)
{/*Is Signal Mask bit is set ?*/
	if(*m == 1)
	{/* Wait for signal delivery */	
		sigsuspend(&mask2);
		sendToAll(6, threadID, sigMask);
		printf("Thread 4 active\n");
	}
	return NULL;
}

void* thread5(int* m)
{/*Is Signal Mask bit is set ?*/
	if(*m == 1)
	{/* Wait for signal delivery */			
		sigsuspend(&mask2);
		sendToAll(6, threadID, sigMask);
		printf("Thread 5 active\n");
	}
	return NULL;
}

void* thread6(int* m)
{/*Is Signal Mask bit is set ?*/
	if(*m == 1)
	{/* Wait for signal delivery */		
		sigsuspend(&mask2);
		sendToAll(6, threadID, sigMask);
		printf("Thread 6 active\n");
	}
	return NULL;
}

/* Signal handler function for SIGIO - executed when signal is recieved */
void Signal_Handler(int signo)
{
	printf("Inside signal handler\n");
}

int filedescriptor; 


int main(int argc,char* argv[])
{
	sigset_t set;
	
	/* Set up the signal Handler */
	sa.sa_handler = Signal_Handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	if((filedescriptor = open(MOUSEFILE, O_RDONLY)) == -1)
	{
		perror("opening device");
		exit(EXIT_FAILURE);
	}

	fcntl(filedescriptor, F_SETOWN, getpid());
	fcntl(filedescriptor, F_SETFL, FASYNC);

	if(sigaction(SIGIO, &sa, NULL) == -1)
		error("sigaction");

	/*Fill the SIGIO to set*/
	sigfillset(&mask2);
	sigdelset(&mask2,SIGIO);

	/*Blocking the main thread from receiving SIGIO */
	sigemptyset(&set);
	sigaddset(&set,SIGIO);

	if(pthread_sigmask(SIG_BLOCK,&set,NULL))
	{
		error("pthread sigmask");
	}

	/* Create the pthreads */
	pthread_create(&threadID[0], NULL,(void *)&thread1,&sigMask[0] );
	pthread_setname_np(threadID[0],"Thread1");
	pthread_create(&threadID[1], NULL,(void *)&thread2,&sigMask[1] );
	pthread_setname_np(threadID[1],"Thread2");
	pthread_create(&threadID[2], NULL,(void *)&thread3,&sigMask[2] );
	pthread_setname_np(threadID[2],"Thread3");
	pthread_create(&threadID[3], NULL,(void *)&thread4,&sigMask[3] );
	pthread_setname_np(threadID[3],"Thread4");
	pthread_create(&threadID[4], NULL,(void *)&thread5,&sigMask[4] );
	pthread_setname_np(threadID[4],"Thread5");
	pthread_create(&threadID[5], NULL,(void *)&thread6,&sigMask[5] );
	pthread_setname_np(threadID[5],"Thread6");
	
	printf("Press right click mouse button for sigio event \n");

	pthread_join(threadID[0], NULL);
	pthread_join(threadID[1], NULL);
	pthread_join(threadID[2], NULL);
	pthread_join(threadID[3], NULL);
	pthread_join(threadID[4], NULL);
	pthread_join(threadID[5], NULL);

	exit(EXIT_SUCCESS);
}


