
/* Course : CSE 438 - Embedded Systems Programming
 * Assignment 2 : To implement GPIO control in Kernel Space
 * Team Member1 : Samruddhi Joshi  ASUID : 1213364722
 * Team Member2 : Varun Joshi 	   ASUID : 1212953337 
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <poll.h>
#include <linux/input.h> /* required for mouse events*/
#include <math.h>
#include <errno.h>
#include "led.h"


#define CYCLE_DURATION 20
#define SEQUENCE_DURATION 500000
#define MOUSEFILE "/dev/input/event3" /* Change the event# as per the target machine */
#define MOUSE 0x01 		//EV_KEY
#define RIGHT_CLICK 0x111 	//BUTTON_RIGHT
#define LEFT_CLICK 0x110 	//BUTTON_LEFT
#define KEY_PRESS 0x1		//EV_VALUE

static int Intensity_Val;
 
int Red_Led;
int Green_Led;
int Blue_Led;
int Exit_Flag =0;
int fd1;

int sequence_count=0;

/* Sequence table
 * R  G  B 
 * 1  0  0 = 0x04
 * 0  1  0 = 0x02
 * 0  0  1 = 0x01
 * 1  1  0 = 0x06
 * 0  1  1 = 0x03
 * 1  0  1 = 0x05
 * 1  1  1 = 0x07 */


void Sequence(void);
void* CheckMouseEvent(void*);


/* Check for mouse event */
/* Description : Thread polls for mouse click event and signals to exit once mouse button is clicked */
void* CheckMouseEvent(void *p)
{
	int fd;
	struct input_event click;

	
	if((fd = open(MOUSEFILE, O_RDONLY)) == -1)
	{
		perror("opening device");
		exit(EXIT_FAILURE);
	}


	while(read(fd, &click, sizeof(struct input_event)))
	{
		if(click.type == MOUSE)  
		{
			if(((click.code == RIGHT_CLICK) && (click.value == KEY_PRESS)) || ((click.code == LEFT_CLICK) && (click.value == KEY_PRESS)))
			{/* Left or Right click event occured, set flag */
				Exit_Flag = 1;
				close(fd1);
				break;
			}
			
		}
	}
return NULL;	
}


/* Description : The below function executes the led lighting sequence 'R,G,B,RG,RB,GB,RGB' 
 * with a gap of 500 ms after each sequence */
void Sequence()
{
	
	int a;
	int* sequence_value=&a;
	
	switch(sequence_count)
	{
		case 0:	
			/*Only Red LED Glows*/
				*sequence_value = 0x04; //Refer Sequence table for values
				write(fd1,(char *)sequence_value,sizeof(int));
				printf("Sequence 1 Red LED ON\n");
				usleep(SEQUENCE_DURATION); 
				
			sequence_count++;
			break;
		case 1:
			/*Only Green LED Glows*/
				*sequence_value = 0x02;
				write(fd1,(char *)sequence_value,sizeof(int));
				printf("Sequence 2 Green LED On\n");
				usleep(SEQUENCE_DURATION); 

			sequence_count++;
			break;
		case 2:
			/*Only Blue LED Glows*/
				*sequence_value = 0x01;
				write(fd1,(char *)sequence_value,sizeof(int));
				printf("Sequence 3 Blue LED on\n");
				usleep(SEQUENCE_DURATION);  
								
			sequence_count++;
			break;
		case 3:
			/*Red and Green LED Glows*/
				*sequence_value = 0x06;
				write(fd1,(char *)sequence_value,sizeof(int));
				printf("Sequence 4 Red Green LED\n");
				usleep(SEQUENCE_DURATION); 
			
			sequence_count++;
			break;
		case 4:
			/*Blue and Green LED Glows*/
				*sequence_value = 0x03;
				write(fd1,(char *)sequence_value,sizeof(int));
				printf("Sequence 5 Blue Green LED\n");
				usleep(SEQUENCE_DURATION); 
			
			sequence_count++;
			break;
		case 5:
			/*Blue and Red LED Glows*/
				*sequence_value = 0x05;
				write(fd1,(char *)sequence_value,sizeof(int));
				printf("Sequence 6 Red Blue LED\n");
				usleep(SEQUENCE_DURATION); 
							
			sequence_count++;
			break;
		case 6:
			/*Blue, Red and Green LED Glows*/
				*sequence_value = 0x07;
				write(fd1,(char *)sequence_value,sizeof(int));
				printf("Sequence 7 Blue Red Green LED\n");
				usleep(SEQUENCE_DURATION); 
				
			sequence_count=0;
			break;
 				default:
	 			break;
	}
 }


int main(int argc, char** argv)
{
	int rerror;
	int time_on;
	struct User_Input* I;
	I = (struct User_Input *)malloc(sizeof(struct User_Input));
	pthread_t thread_ID;    /* daemon thread to check for mouse event */
	pthread_attr_t thread_attr;
	int thread_priority=50;  /* Mouse detect */
	struct sched_param param;
	int err;

	/* Extract the Pins and Duty cycle */
	sscanf(argv[1],"%i", &Intensity_Val);
	sscanf(argv[2],"%i", &Red_Led);
	sscanf(argv[3],"%i", &Green_Led);
	sscanf(argv[4],"%i", &Blue_Led);

	if((Intensity_Val>100)||(Intensity_Val<0))
	{
		printf("Incorrect Intensity Parameter, Allowed value is 0-100\n");
		exit(0);
	}
	
	time_on = ((CYCLE_DURATION*0.01)*(Intensity_Val));
	
	I->Pin_Intensity = time_on;
	I->Pin_Red = Red_Led;
	I->Pin_Green = Green_Led;
	I->Pin_Blue = Blue_Led;

	/* Initialise the GPIO Pins */
	fd1 = open("/dev/RGBLed",O_RDWR);
	/* Configure the GPIO Pins */
	err = ioctl(fd1,CONFIG,&I);
	if(err < 0)
			{
			errno = EINVAL;
			printf("Invalid Arguments");
			exit(0);
			}

	/* Create a thread to montior mouse event */
	pthread_attr_init(&thread_attr);
	pthread_attr_getschedparam(&thread_attr, &param);
	param.sched_priority = thread_priority;  /* set thread priority */
	pthread_attr_setschedparam(&thread_attr, &param);
	pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
	rerror = pthread_create(&thread_ID, &thread_attr, &CheckMouseEvent, NULL);
	if(rerror != 0)
		{
		printf("Error while creating daemon thread \n");
		}
	
	while(Exit_Flag == 0)
	{/* While no mouse event occurs, execute the sequence */
		Sequence();
	}
	
	pthread_join(thread_ID,NULL);

	return 0;
}
