
/* Course : CSE 438 - Embedded Systems Programming
 * Assignment 1b : To implement Shared Message queues in Kernel Space
 * Team Member1 : Samruddhi Joshi  ASUID : 1213364722
 * Team Member2 : Varun Joshi 	   ASUID : 1212953337 */



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

#define MOUSEFILE "/dev/input/event4" /* Change the event# as per the target machine */
#define MOUSE 0x01 		//EV_KEY
#define RIGHT_CLICK 0x111 	//BUTTON_RIGHT
#define LEFT_CLICK 0x110 	//BUTTON_LEFT
#define KEY_PRESS 0x1		//EV_VALUE
#define BASE_PERIOD 1000
#define MSEC 1000
#define CPU_CLOCK_FREQUENCY 400000
#define MAX 100000

int receiver_period = 40;  /* Periodicity of Receiver thread */
int periodic_period[4] = {12,32,18,28}; /* Periodicity of Periodic threads */
queue *data_queue1, *data_queue2;
int Message_count,Drop_Counter,Receive_Count;
int random_val;
int Total_Count,Tx_Count,Rx_Count;
long double queueing_time[MAX];
sem_t Sem_Right;
sem_t Sem_Left;

volatile int Terminate_Flag = 0;
int StopReceive_flag = 0;

/* Function declarations */
void* Periodic_Threadfunc(void *ptr);
void* Daemon_Threadfunc(void *ptr);
void* Aperiodic_Threadfunc(void *ptr);
void* Reciever_Threadfunc(void *ptr);
double CalculatePi(int iteration);
int RandomNum(void);

double Average=0, Std_dev=0;
static double Sum = 0;
static double sum_diff_square= 0;

pthread_mutex_t Mutex_DQ = PTHREAD_MUTEX_INITIALIZER;

int main()
{
srand(time(NULL));

#ifdef AFFINITY
cpu_set_t cpus;
CPU_ZERO(&cpus); 
CPU_SET(1,&cpus);
int affinity_return;
#endif


pthread_t thread_ID[8];    /* 4 periodic, 2 aperiodic, 1 receiver, 1 daemon */
pthread_attr_t thread_attr[8];
int thread_priority[]={50,50,50,50,60,61,61,40};  /* P1, P2, P3, P4, D, AP1, AP2, Rx */
struct sched_param param[8];
int rerror[8]; /* to check for error in pthread creation */
int i;

int periodic[4] = {0,1,2,3};
const int* pthread[5] = {&periodic[0], &periodic[1],&periodic[2],&periodic[3]};  /* pointer to periodic thread */

int d_id[1] = {0};
const int* did[1] = {&d_id[0]}; /* pointer to daemon thread */

int a_id[7] = {0,0,0,0,0,0,1};
const int* aid[7] = {&a_id[0], &a_id[1],&a_id[2], &a_id[3],&a_id[4], &a_id[5],&a_id[6]}; /* pointer to aperiodic thread */

int r_id[1] = {0};
const int* rid[1] = {&r_id[0]}; /* pointer to receiver thread */


	/* Initialise the Semaphores */
	sem_init(&Sem_Right,0,0);
	sem_init(&Sem_Left,0,0);

	/* Create threads independently for each Periodic, Aperiodic and Rx */
	/* Periodic threads P1, P2, P3, P4 */
	for(i=0; i<4; i++)
	{
	pthread_attr_init(&thread_attr[i]);
	#ifdef AFFINITY
	pthread_attr_setaffinity_np(&thread_attr[i],sizeof(cpu_set_t),&cpus);
	#endif
	pthread_attr_getschedparam(&thread_attr[i], &param[i]);
	param[i].sched_priority = thread_priority[i];  /* set thread priority */
	pthread_attr_setschedparam(&thread_attr[i], &param[i]);
	pthread_setschedparam(pthread_self(), SCHED_FIFO, &param[i]);
	rerror[i] = pthread_create(&thread_ID[i], &thread_attr[i], &Periodic_Threadfunc, (void*) pthread[i]);	
	if(rerror[i] != 0)
		{
		printf("Error while creating periodic thread %d \n",i+1);
		}
	}

	/* Daemon thread for mouse event */
	pthread_attr_init(&thread_attr[4]);
	#ifdef AFFINITY
	pthread_attr_setaffinity_np(&thread_attr[i],sizeof(cpu_set_t),&cpus);
	#endif
	
	pthread_attr_getschedparam(&thread_attr[4], &param[4]);
	param[4].sched_priority = thread_priority[4];  /* set thread priority */
	pthread_attr_setschedparam(&thread_attr[4], &param[4]);
	pthread_setschedparam(pthread_self(), SCHED_FIFO, &param[4]);
	rerror[4] = pthread_create(&thread_ID[4], &thread_attr[4], &Daemon_Threadfunc, (void*) did[0]);
	if(rerror[4] != 0)
		{
		printf("Error while creating daemon thread \n");
		}

	/* Aperiodic thread AP1, AP2 */
	for(i=5; i<7; i++)
	{
	pthread_attr_init(&thread_attr[i]);
	#ifdef AFFINITY
	pthread_attr_setaffinity_np(&thread_attr[i],sizeof(cpu_set_t),&cpus);
	#endif
	
	pthread_attr_getschedparam(&thread_attr[i], &param[i]);
	param[i].sched_priority = thread_priority[i];  /* set thread priority */
	pthread_attr_setschedparam(&thread_attr[i], &param[i]);
	pthread_setschedparam(pthread_self(), SCHED_FIFO, &param[i]);
	rerror[i] = pthread_create(&thread_ID[i], &thread_attr[i], &Aperiodic_Threadfunc, (void*) aid[i]);
	if(rerror[i] != 0)
		{
		printf("Error while creating aperiodic thread %d\n",i+1);
		}
	}	

	/* Receiver thread R */
	pthread_attr_init(&thread_attr[7]);
	#ifdef AFFINITY
	pthread_attr_setaffinity_np(&thread_attr[i],sizeof(cpu_set_t),&cpus);
	#endif
	
	pthread_attr_getschedparam(&thread_attr[7],&param[7]);
	param[7].sched_priority=thread_priority[7];   /* set thread priority */
	pthread_attr_setschedparam(&thread_attr[7],&param[7]);
	pthread_setschedparam(pthread_self(), SCHED_FIFO, &param[7]);
	rerror[7] = pthread_create(&thread_ID[7],&thread_attr[7],&Reciever_Threadfunc,(void*) rid[0]);
	if(rerror[7] != 0)
		{
		printf("Error while creating receiver thread \n");
		}

	
	for(i=0; i<=7; i++)
	{
	pthread_join(thread_ID[i],NULL); /* wait for all threads to terminate */
	}
	printf("*****************************************\n");
	printf("Total Messages = %d Messages Received= %d Messages Transmitted=%d Messages Dropped=%d \n",Message_count,Rx_Count,Tx_Count,(Message_count-Rx_Count));

	/* Calculation of Standard Deviation */
	
	for(i=0;i<Rx_Count;i++)
	{
	Sum+=queueing_time[i];
	}
	Average = (Sum/Rx_Count);
	
	for(i=0;i<Rx_Count;i++)
	{
		sum_diff_square+=pow(queueing_time[i]-Average,2);
	}

        Std_dev = sqrt(sum_diff_square/Rx_Count);
	printf("*****************************************\n");
	printf("Average of message queueing time is %f ms\n",Average);
	printf("*****************************************\n");
	printf("Standard deviation is %f ms \n",Std_dev);
	printf("*****************************************\n");


	exit(0);

}


/* Description : Reciever thread reads messages from both the dataqueues */
void* Reciever_Threadfunc(void *ptr)
{
	 struct timespec next, period;
	 int Err1,Err2,fd1,fd2;
	 struct message* m1;	
	 struct message* m2;
	 clock_gettime(CLOCK_MONOTONIC, &next);  /* captures the current time */
	 period.tv_nsec =0;
	 period.tv_nsec = receiver_period*BASE_PERIOD*1000;  
	 m1 = (struct message *) malloc(sizeof(struct message));
	 m2 = (struct message *) malloc(sizeof(struct message));

	 while(!(Terminate_Flag) || (StopReceive_flag != 1)) // While the termination flag is not set and buffer is not empty
	 {	
		 fd1 = open("/dev/DataQueue1",O_RDWR);
		 fd2 = open("/dev/DataQueue2",O_RDWR);
		 Err1 = read(fd1,(char *)m1,sizeof(struct message));
		 m1 = (struct message*)m1;
		 if(Err1 == 0) /*If DQ1 is not empty, read the message content */
		 	{
		 	pthread_mutex_lock(&Mutex_DQ); /*Secure increment with mutex lock as it a shared resource */
		 	(++Rx_Count);
		 	queueing_time[Rx_Count-1] = (long double)m1->queue_time/CPU_CLOCK_FREQUENCY;
		 	pthread_mutex_unlock(&Mutex_DQ);
		 	printf("Message received in DQ1 message_id=%d source_id=%d message=%1.16lf \n",m1->message_id,m1->source_id,m1->pi_value);
		 	printf("Message queue time is %Lf ms\n",queueing_time[Rx_Count-1]);
		 	}
			

		 Err2 = read(fd2,(char *)m2,sizeof(struct message));
		 m2 = (struct message*)m2;
		 if(Err2 == 0) //If DQ2 is not empty, read the message content 
		 	{
		 	pthread_mutex_lock(&Mutex_DQ); /*Secure increment with mutex lock as it a shared resource */
		 	(++Rx_Count);
		 	queueing_time[Rx_Count-1] = (long double)m2->queue_time/CPU_CLOCK_FREQUENCY;
		 	pthread_mutex_unlock(&Mutex_DQ);
		 	printf("Message received in DQ2 message_id=%d source_id=%d message=%1.16lf \n",m2->message_id,m2->source_id,m2->pi_value);
		 	printf("Message queue time is %Lf ms\n",queueing_time[Rx_Count-1]);
		 	}
			
		 if((Err1 == -1 && Err2 == -1) && Terminate_Flag == 1)
		 	{/* If both the Data Queues are empty and flag for termination is set, exit */		
		 	StopReceive_flag = 1; /* Do not read any more messages as queue is empty and termination initiated */
		 	}
		 close(fd1);
		 close(fd2);

		next.tv_nsec = next.tv_nsec + period.tv_nsec; /* add the time you want the thread to sleep */

		/* Normalise the time to account for second boundary */
		if(next.tv_nsec >= 1000000000){
			next.tv_nsec = (next.tv_nsec % 1000000000);
			next.tv_sec++;
			}

		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, 0); /* Wait until next period */
	 }
return(NULL);
}	

/* Description : Periodic threads send messages to corresponding Data queue after equal intervals 
 * P1,P2 enqueue messages to Data Queue1 and P3,P4 enqueue messages to DataQueue2 */
void* Periodic_Threadfunc(void *ptr){
	struct timespec next, period;
	int* source_id;
	double Value;
	int Err;
	int fd1,fd2;
	source_id = (int *) ptr;
	struct message* Msg;
	Msg = (struct message *) malloc(sizeof(struct message)); /* Allocate memory for pointer to message */
		
	clock_gettime(CLOCK_MONOTONIC, &next);  /* captures the current time */
	period.tv_nsec =0;
	period.tv_nsec = periodic_period[*source_id]*BASE_PERIOD*1000;  /*calculate period for corresponding thread*/

	while(!Terminate_Flag) /* While termination sequence is not initiated */
	{

		fd1 = open("/dev/DataQueue1",O_RDWR);
		fd2 = open("/dev/DataQueue2",O_RDWR);
		
		/* Update Message Content */
		Msg->source_id = *source_id;
		Msg->queue_time = 0;

		/* Increment the message id after mutex lock as it is shared resource */
		pthread_mutex_lock(&Mutex_DQ);
		(Msg->message_id=++Message_count);
		pthread_mutex_unlock(&Mutex_DQ);

		/* calculate Pi value */
		random_val = RandomNum();
		Value = CalculatePi(random_val);
		Msg->pi_value = Value;
		
		printf("Message being sent by Periodic thread %d for Message id %d is %1.16lf\n",(Msg->source_id+1),Msg->message_id,Msg->pi_value);
		
		
		if((*source_id == 0) || (*source_id == 1)) /* Depending on the thread no, buffer is selected */
			Err = write(fd1,(char *)Msg,sizeof(struct message));  
		else
			Err = write(fd2,(char *)Msg,sizeof(struct message));  

		if(Err < 0)
			{
			errno = EINVAL;
			}
		else
			{
			/* Increment the tx count after mutex lock as it is shared resource */
			pthread_mutex_lock(&Mutex_DQ);
			Tx_Count++;
		      	pthread_mutex_unlock(&Mutex_DQ);
			}

		close(fd1);
		close(fd2);
		
		next.tv_nsec = next.tv_nsec + period.tv_nsec; 
		/* Normalise the time to account for second boundary */
		if(next.tv_nsec >= 1000000000){
			next.tv_nsec = (next.tv_nsec % 1000000000);
			next.tv_sec++;
			}
		
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, 0);
	}
	free(Msg);
	return(NULL);
}

/* Description : Daemon thread polls for mouse click event and signals to corresponding
 * aperiodic threads once mouse button is clicked */
void* Daemon_Threadfunc(void *ptr)
{
	int filedescriptor; 
	struct input_event click;
	unsigned long int mouse_click1, mouse_click2, final_current, final_prev;
	
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
				sem_post(&Sem_Right);
			}
			else if((click.code == LEFT_CLICK) && (click.value == KEY_PRESS))
			{
				/* Left click event occured */
				mouse_click1 = click.time.tv_sec*1000; //second value
				mouse_click2 = click.time.tv_usec/MSEC; //ms value
				final_current = mouse_click1 + mouse_click2;
				if((final_current - final_prev)<500)
					{
						/* On double click detect, Enter the termination sequence */
						Terminate_Flag = 1; 
						sem_post(&Sem_Left);
						sem_post(&Sem_Right);
						break;
					}
				else
					{sem_post(&Sem_Left);}

				final_prev = final_current; /* Store for next click event */
			}
			else 
			{}
		}	
	}
	return(NULL);
}

/* Description: Aperiodic threads send messages to corresponding Data queue after event detect
 * AP1 - Sends message to Data Queue 1 once Right Click mouse event is detected
 * AP2 - Sends message to Data Queue 2 once Left Click mouse event is detected  */
void* Aperiodic_Threadfunc(void *ptr)
{
	int* source_id;
	int Err;
	double Value;
	source_id = (int *) ptr;
	int fd1,fd2;
	struct message* Msg;
	Msg = (struct message *) malloc(sizeof(struct message)); /* Allocate memory for pointer to message */	

	while (!Terminate_Flag) /* While termination sequence is not initiated */
	{
	fd1 = open("/dev/DataQueue1",O_RDWR);
	fd2 = open("/dev/DataQueue2",O_RDWR);
	if(*source_id == 0) 
		{
		sem_wait(&Sem_Right); /* Wait till Right click event occurs */
			
		/* Update Message Content */
		Msg->source_id = *source_id;
		Msg->queue_time = 0;
		/* Increment the message id */
		pthread_mutex_lock(&Mutex_DQ);
		(Msg->message_id=++Message_count);
		pthread_mutex_unlock(&Mutex_DQ);
		/* calculate Pi value */
		random_val = RandomNum();
		Value = CalculatePi(random_val);
		Msg->pi_value = Value;
		printf("Message being sent by Aperiodic thread %d for Message id %d is %1.16lf\n",(Msg->source_id+1),Msg->message_id,Msg->pi_value);
		Err = write(fd1,(char *)Msg,sizeof(struct message));  
		if(Err < 0)
			{
			errno = EINVAL;
			}
		else
			{/* Increment successful transmit count */
			pthread_mutex_lock(&Mutex_DQ);
			Tx_Count++;
			pthread_mutex_unlock(&Mutex_DQ);
			}
		}
	else
		{	
		sem_wait(&Sem_Left); /* Wait till Left click event occurs */
	
		/* Update Message Content */
		Msg->source_id = *source_id;
		Msg->queue_time = 0;
		/* Increment the message id */
		pthread_mutex_lock(&Mutex_DQ);
		(Msg->message_id=++Message_count);
		pthread_mutex_unlock(&Mutex_DQ);
		/* calculate Pi value */
		random_val = RandomNum();
		Value = CalculatePi(random_val);
		Msg->pi_value = Value;
		printf("Message being sent by Aperiodic thread %d for Message id %d is %1.16lf\n",(Msg->source_id+1),Msg->message_id,Msg->pi_value);
		Err = write(fd2,(char *)Msg,sizeof(struct message));  
		if(Err < 0)
			{
			errno = EINVAL;
			}
		else
			{/* Increment successful transmit count */
			pthread_mutex_lock(&Mutex_DQ);
			Tx_Count++;
			pthread_mutex_unlock(&Mutex_DQ);
			}
		}
	close(fd1);
	close(fd2);
	}
	free(Msg);
	return(NULL);
}

/* Description : CalculatePi() computes with iterative approximation the value of number PI 
 * Arguments   : int - The Random number generated is passed as number of iterations for computation 
 * Return Value: double - computed PI value 
 * Reference   : Leibniz's Series from Code Project */
double CalculatePi(int iteration)
{
	double n, i;       // Number of iterations and control variable
   	double s = 1;      //Signal for the next iteration
   	double pi = 0;
 	n = iteration;
	
	for(i = 1; i <= (n * 2); i += 2){
	     pi = pi + s * (4 / i);
	     s = -s;
	   }
	
	return pi;
}

/* Description : RandomNum() generates random number within given range 
 * Arguments   : void 
 * Return Value: int - random var */
int RandomNum(void)
{
    int result = 0, low_num = 0, hi_num = 0;
    int	min_num = 10, max_num = 50;
    
    if (min_num < max_num)
    {
        low_num = min_num;
        hi_num = max_num + 1; // include max_num in output
    } else {
        low_num = max_num + 1; // include max_num in output
        hi_num = min_num;
    }
    
    srand(time(NULL));
    result = (rand() % (hi_num - low_num)) + low_num;
    return result;
}



