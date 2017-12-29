/* Course : CSE 438 - Embedded Systems Programming
 * Assignment 1 : To implement Shared Message queues in User Space
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
#include "queue.h"        /* Own header */

/* Function to read Time Stamp Counter 
*  Return Value is in cpu cycles */
static __inline__ unsigned long long rdtsc(void)
{
    unsigned long lo, hi;
    asm( "rdtsc" : "=a" (lo), "=d" (hi) ); 
    return( lo | (hi << 32) );
}

/***************** Queue Functions ****************/
/*    f
 *  ----------------------------------------
 *  |   |   |   |   |   |   |   |   |   |   |
 *  ----------------------------------------
 *    r
 *
 *  f = front (write operation)
 *  r = rear (read operation)
 *  Increment the front after every write and increment rear after every read */

/* Description : Function to create a Data Queue 
 * Arguments   : void 
 * Return Value: Pointer to Queue created */
queue* sq_create()
{
	queue* queue_new = malloc(sizeof(queue)); /* Dynamically allocate size for new queue */
	/* Initialize the front, rear and full*/
	queue_new->front = 0;
	queue_new->rear	 = 0;
	queue_new->full  = 0;
	pthread_mutex_init(&(queue_new->Mutex_DQ), NULL);
	return queue_new;
}

/* Description : Function to write into a particular Data Queue 
 * Arguments   : Dataqueue in which to be written, Message pointer
 * Return Value: Write successful 0, unsuccessful -1 */
int sq_write(queue *dataqueue,struct message *m1)
{	
	int index;
	
	pthread_mutex_lock(&(dataqueue->Mutex_DQ));
	/* Check whether Data Queue is full */
	if((dataqueue->rear==dataqueue->front) && (dataqueue->full))
		{
		pthread_mutex_unlock(&(dataqueue->Mutex_DQ));
		return -1; /* Cannot write into queue */
		}
	index = dataqueue->front; 
	dataqueue->message_pointer[index] = m1;
	/* Capture Enqueue time */
	dataqueue->message_pointer[index]->queue_time = rdtsc();
	/* Increment the front to next write location */	
	dataqueue->front++; 
	/* To make the buffer circular */
	dataqueue->front = dataqueue->front % QUEUE_LEN;
	if(dataqueue->rear==dataqueue->front)
		dataqueue->full  = 1; /* Set flag as buffer is full */
	pthread_mutex_unlock(&(dataqueue->Mutex_DQ));
	return 0;
}

/* Description : Function to from a particular Data Queue 
 * Arguments   : Dataqueue to read from, Message pointer
 * Return Value: Read successful 0, unsuccessful -1 */
int sq_read(queue *dataqueue,struct message **m1)
{
	int index;
	long double current_time; 
	pthread_mutex_lock(&(dataqueue->Mutex_DQ));
   	/* Check whether Data Queue is empty */
	if((dataqueue->rear==dataqueue->front) && !(dataqueue->full))
		{
		*m1 = NULL; /* Nothing to read */
		pthread_mutex_unlock(&(dataqueue->Mutex_DQ));
		return -1;
		}
	index = dataqueue->rear;
	/* Calculate the dequeue time = Current time - Enqueued time */
	current_time = rdtsc();
	dataqueue->message_pointer[index]->queue_time = (current_time - (dataqueue->message_pointer[index]->queue_time));
	*m1 = dataqueue->message_pointer[index];
	/* Increment the front to next read location */
	dataqueue->rear++;
	/* To make the buffer circular */
	dataqueue->rear = dataqueue->rear % QUEUE_LEN;
	if(dataqueue->rear == dataqueue->front)
		dataqueue->full = 0; /* Set flag as buffer is empty */
	pthread_mutex_unlock(&(dataqueue->Mutex_DQ));
	return 0;
}

/* Description : Function to delete a particular Data Queue 
 * Arguments   : Dataqueue to delete
 * Return Value: void */
void sq_delete(queue* dataqueue){
free(dataqueue);
}


