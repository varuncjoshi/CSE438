#include "lib.h"
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

void sendToAll(int m, pthread_t t[], int sigMask[])
{/*If value received from one thread, signal sent to all registered threads */
	int value;
	for (value = 0; value < m; value++)
	{
		if(sigMask[value] == 1)
		{/* If the thread is registered, send signal */
			pthread_kill(t[value], SIGIO);
		}
	}
}