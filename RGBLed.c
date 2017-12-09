
/* Course : CSE 438 - Embedded Systems Programming
 * Assignment 2 : To implement GPIO control in User Space
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
#include "led.h"


#define CYCLE_DURATION 20
#define SEQUENCE_DURATION 500
#define MOUSEFILE "/dev/input/event2" /* Change the event# as per the target machine */
#define MOUSE 0x01 		//EV_KEY
#define RIGHT_CLICK 0x111 	//BUTTON_RIGHT
#define LEFT_CLICK 0x110 	//BUTTON_LEFT
#define KEY_PRESS 0x1		//EV_VALUE

static int Intensity_Val;
 
int Red_Led;
int Green_Led;
int Blue_Led;
int Duty_Cycle;
int p_r, d_r, m1_r, m2_r;
int p_g, d_g, m1_g, m2_g;
int p_b, d_b, m1_b, m2_b;
int Exit_Flag =0;

struct Pin_Mapping
{
	int Pin;
	int Dir;
	int Mux1;
	int Mux1_Val;
	int Mux2;
	int Mux2_Val;
}GpioPin[14] = {
/*	Pin Dir Mux1 Val Mux2 Val */
	{11, 32,  0, 0,  0, 0}, //IO0
	{12, 28, 45, 0,  0, 0}, //IO1
	{13, 34, 77, 0,  0, 0}, //IO2
	{14, 16, 76, 0, 64, 0}, //IO3
	{ 6, 36,  0, 0,  0, 0}, //IO4
	{ 0, 18, 66, 0,  0, 0}, //IO5
	{ 1, 20, 68, 0,  0, 0}, //IO6
	{38,  0,  0, 0,  0, 0}, //IO7
	{40,  0,  0, 0,  0, 0}, //IO8
	{ 4, 22, 70, 0,  0, 0}, //IO9
	{10, 26, 74, 0,  0, 0}, //IO10
	{ 5, 24, 44, 0, 72, 0}, //IO11
	{15, 42,  0, 0,  0, 0}, //IO12
	{ 7, 30, 46, 0,  0, 0}  //IO13
};


int sequence_count=0;


void Sequence(void);
void* CheckMouseEvent(void*);
void IOSetup(void);


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
				break;
			}
			
		}
	}
return NULL;	
}


/* Description : The below function executes the led lighting sequence 'R,G,B,RG,RB,GB,RGB' */
void Sequence(void)
{
	
	
	int loop_i;
	int loop_count;
	int on_time, off_time;
	/* Calculate the ON/OFF duration as per the Intensity Value passed by the user */
	on_time=((CYCLE_DURATION*0.01)*(Intensity_Val));
	off_time = (CYCLE_DURATION - on_time);
	loop_count = SEQUENCE_DURATION/CYCLE_DURATION;


	switch(sequence_count)
	{
		case 0:
			for(loop_i =0; loop_i < loop_count; loop_i++)
			{/*Only Red LED Glows*/
				gpio_set_value(p_r,1);
				printf("Sequence 1 Red LED\n");
				usleep(on_time*1000); 
				gpio_set_value(p_r,0);
				usleep(off_time*1000);
			}
			sequence_count++;
			break;
		case 1:
			for(loop_i =0; loop_i < loop_count; loop_i++)
			{/*Only Green LED Glows*/
				gpio_set_value(p_g,1);
				printf("Sequence 2 Green LED\n");
				usleep(on_time*1000); 
				gpio_set_value(p_g,0);
				usleep(off_time*1000);
			}
			sequence_count++;
			break;
		case 2:
			for(loop_i =0; loop_i < loop_count; loop_i++)
			{/*Only Blue LED Glows*/
				gpio_set_value(p_b,1);
				printf("Sequence 3 Blue LED\n");
				usleep(on_time*1000); 
				gpio_set_value(p_b,0);
				usleep(off_time*1000);
			}
			sequence_count++;
			break;
		case 3:
			for(loop_i =0; loop_i < loop_count; loop_i++)
			{/*Red and Green LED Glows*/
				gpio_set_value(p_r,1);
				gpio_set_value(p_g,1);
				printf("Sequence 4 Red Green LED\n");
				usleep(on_time*1000); 
				gpio_set_value(p_r,0);
				gpio_set_value(p_g,0);
				usleep(off_time*1000);
			}
			sequence_count++;
			break;
		case 4:
			for(loop_i =0; loop_i < loop_count; loop_i++)
			{/*Blue and Green LED Glows*/
				gpio_set_value(p_b,1);
				gpio_set_value(p_g,1);
				printf("Sequence 5 Blue Green LED\n");
				usleep(on_time*1000); 
				gpio_set_value(p_b,0);
				gpio_set_value(p_g,0);
				usleep(off_time*1000);
			}
			sequence_count++;
			break;
		case 5:
			for(loop_i =0; loop_i < loop_count; loop_i++)
			{/*Blue and Red LED Glows*/
				gpio_set_value(p_r,1);
				gpio_set_value(p_b,1);
				printf("Sequence 6 Red Blue LED\n");
				usleep(on_time*1000); 
				gpio_set_value(p_r,0);
				gpio_set_value(p_b,0);
				usleep(off_time*1000);
			}
			sequence_count++;
			break;
		case 6:
			for(loop_i =0; loop_i < loop_count; loop_i++)
			{/*Blue, Red and Green LED Glows*/
				gpio_set_value(p_r,1);
				gpio_set_value(p_b,1);
				gpio_set_value(p_g,1);
				printf("Sequence 7 Blue Red Green LED\n");
				usleep(on_time*1000); 
				gpio_set_value(p_r,0);
				gpio_set_value(p_b,0);
				gpio_set_value(p_g,0);
				usleep(off_time*1000);
			}
			sequence_count=0;
			break;
		default:
			break;
	}
}

/* Description : The below function initialises the GPIO pin direction, Pin Multiplexing for the pins inputted by User */
void IOSetup(void)
{
	/*******************************/
	/* Set the Mux for Red LED */
	p_r = GpioPin[Red_Led].Pin;
	m1_r = GpioPin[Red_Led].Mux1;
	m2_r = GpioPin[Red_Led].Mux2;
	d_r = GpioPin[Red_Led].Dir;
	

	/* Set the required Mux */
	mux_gpio_set(d_r,0);
	mux_gpio_set(m1_r,0);
	mux_gpio_set(m2_r,0);

	/*******************************/
	/* Select the Mux for Green LED */
	p_g = GpioPin[Green_Led].Pin;
	m1_g = GpioPin[Green_Led].Mux1;
	m2_g = GpioPin[Green_Led].Mux2;
	d_g = GpioPin[Green_Led].Dir;

	/* Set the required Mux */
	mux_gpio_set(d_g,0);
	mux_gpio_set(m1_g,0);
	mux_gpio_set(m2_g,0);

	/*******************************/
	/* Select the Mux for BLUE LED */
	p_b = GpioPin[Blue_Led].Pin;
	m1_b = GpioPin[Blue_Led].Mux1;
	m2_b = GpioPin[Blue_Led].Mux2;
	d_b = GpioPin[Blue_Led].Dir;

	/* Set the required Mux */
	mux_gpio_set(d_b,0);
	mux_gpio_set(m1_b,0);
	mux_gpio_set(m2_b,0);

	/*******************************/
	/* Export and Set the Pin Direction RED*/
	gpio_export(p_r);
	gpio_set_dir(p_r, 1);
	
	/* Export and Set the Pin Direction GREEN */
	gpio_export(p_g);
	gpio_set_dir(p_g, 1);

	/* Export and Set the Pin Direction BLUE */
	gpio_export(p_b);
	gpio_set_dir(p_b, 1);
	/*******************************/
	
}


int main(int argc, char** argv)
{
	int rerror;
	pthread_t thread_ID;    /* daemon thread to check for mouse event */
	pthread_attr_t thread_attr;
	int thread_priority=50;  /* Mouse detect */
	struct sched_param param;

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
	
	if(((Red_Led<0)||(Red_Led>13))||((Green_Led<0)||(Green_Led>13))||((Blue_Led<0)||(Blue_Led>13)))
	{
		printf("Only GPIO Pins between 0 to 13 allowed\n");
		exit(0);
	}
	
	/* Initialise the GPIO Pins */
	IOSetup();

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
	{
		Sequence();
	}
	
	pthread_join(thread_ID,NULL);

	/* Unexport the Pin RED*/
	gpio_unexport(p_r);
	
	/* Unexport the Pin GREEN */
	gpio_unexport(p_g);
	
	/* Unexport the Pin BLUE */
	gpio_unexport(p_b);

	return 0;
}
