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


#define MOUSEFILE "/dev/input/event7" /* Change the event# as per the machine */
#define MOUSE 0x01 		//EV_KEY
#define RIGHT_CLICK 0x111 	//BUTTON_RIGHT
#define LEFT_CLICK 0x110 	//BUTTON_LEFT
#define KEY_PRESS 0x1		//EV_VALUE
#define BASE_PERIOD 1000
#define MSEC 1000
#define CPU_CLOCK_FREQUENCY 2000000
#define MAX 100000

/* Global Variables Declaration */
pthread_t thread_ID[2];    /* 1 SPI_transmit thread, 1 sensor_detect */
pthread_attr_t thread_attr[2];
int thread_priority[]={50,50};  
struct sched_param param[2];
int rerror[2]; /* to check for error in pthread creation */
pthread_mutex_t lock;
static double computeValue = 2;
static double terms = 3000, ratio = 1.3;
static jmp_buf env;
int flag = 0;
static double sum;

/* Function declarations */
void* Computation_Threadfunc(void *ptr);
void* DoubleClick_Threadfunc(void *ptr);
void signal_handler(int signo);


int main(void)
{
	int i,j;
	
	pthread_mutex_init(&lock, NULL);
	/* define */
	signal(SIGUSR1, signal_handler);

	/* Create two pthreads */
	for(i=0; i<2; i++)
	{
		pthread_attr_init(&thread_attr[i]);
		pthread_attr_getschedparam(&thread_attr[i], &param[i]);
		param[i].sched_priority = thread_priority[i];  /* set thread priority */
		pthread_attr_setschedparam(&thread_attr[i], &param[i]);
		pthread_setschedparam(pthread_self(), SCHED_FIFO, &param[i]);
	}
	
	rerror[1] = pthread_create(&thread_ID[1], &thread_attr[1], &Computation_Threadfunc, NULL);
	pthread_setname_np(thread_ID[1],"Thread_Compute");
	rerror[0] = pthread_create(&thread_ID[0], &thread_attr[0], &DoubleClick_Threadfunc, NULL);	
	pthread_setname_np(thread_ID[0],"Thread_DoubleClick");

	if((rerror[0] != 0) || (rerror[1] != 0))
	{
		printf("Error while creating thread \n");
	}
	/* Store the registers and counters to the buffer */
	j = setjmp(env);

	/* If longjmp event, go to signal handler */
	if(j == -1)	
			raise(SIGUSR1);
	
	for(i=0; i<2; i++)
	{/* wait for all threads to terminate */
		pthread_join(thread_ID[i],NULL); 
	}
	
return 0;
}

/*
 * This Thread computes the Geometric Progression */
void* Computation_Threadfunc(void *ptr)
{
	static int iteration;
	/* print the series and add all elements to sum */

	/* Formula : An = A1 * r^(n-1) 
	computeValue = A1
	ratio = r
	terms = n */
    while(iteration < terms)
    {
        sum += computeValue;
        computeValue = computeValue * ratio;
        printf("computeValue is %f\n",computeValue);
        printf("Sum is %f\n",sum);
        usleep(100000);
        iteration++;
    
        if(flag)
        {/* Will call setjmp with return value -1 */
        	longjmp(env, -1);
        }
    	
    }

printf("Exiting compute thread\n");
return(NULL);
}

/*
 * Signal handler for exception */
void signal_handler(int signo)
{
	printf("The imprecise value of GP is %f\n",sum);
	exit(EXIT_SUCCESS);
}

/*
 * This Thread detects the double click event */
void* DoubleClick_Threadfunc(void *ptr)
{
	int filedescriptor; 
	struct input_event click;
	unsigned long int mouse_click1, mouse_click2, final_current, final_prev;
	
	if((filedescriptor = open(MOUSEFILE, O_RDONLY)) == -1)
	{
		perror("Opening Device");
		exit(EXIT_FAILURE);
	}

	while(read(filedescriptor, &click, sizeof(struct input_event)))
	{
		if(click.type == MOUSE)  
		{
			if((click.code == RIGHT_CLICK) && (click.value == KEY_PRESS))
			{
				// Left click event occured 
				mouse_click1 = click.time.tv_sec*1000;  //second value
				mouse_click2 = click.time.tv_usec/MSEC; //ms value
				final_current = mouse_click1 + mouse_click2;
				if((final_current - final_prev)<300)
					{
						/* On double click detect, abort and print the imprecise computation value */
						flag = 1;
						break;
					}
				final_prev = final_current; /* Store for next click event */
			}
		}	
	}
printf("Exiting click thread\n");
return(NULL);
}
