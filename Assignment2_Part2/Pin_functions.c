/* Course : CSE 438 - Embedded Systems Programming
 * Assignment 2 : To implement GPIO control in Kernel Space
 * Team Member1 : Samruddhi Joshi  ASUID : 1213364722
 * Team Member2 : Varun Joshi 	   ASUID : 1212953337 
*/

#define _GNU_SOURCE
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <asm/msr.h>
#include <linux/string.h>
#include <linux/device.h>
#include <linux/jiffies.h>
#include <linux/semaphore.h>   /* required for semaphores */
#include <linux/sched.h> 
#include <linux/init.h>
#include <linux/mutex.h> 
#include <linux/time.h> 
#include <linux/moduleparam.h>
#include <linux/gpio.h>
#include <linux/ioctl.h>
#include <linux/timer.h>   /* Needed for timer */
#include <linux/spinlock.h>
#include <linux/hrtimer.h>

#define CONFIG _IOR('I', 1, struct User_Input)
/* ----------------------------------------------- DRIVER led --------------------------------------------------
 
 LED driver to show skelton methods for several file operations.
 
 ----------------------------------------------------------------------------------------------------------------*/



#define DEVICE_NAME    "RGBLed"  // device name to be created and registered

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

static struct gpio request_gpio_arr[] = 
{
	{11, 0, "GPIO11"},
	{32, 0, "GPIO32"},
	{12, 0, "GPIO12"},
	{28, 0, "GPIO28"},
	{45, 0, "GPIO45"},
	{13, 0, "GPIO13"},
	{34, 0, "GPIO34"},
	{14, 0, "GPIO14"},
	{16, 0, "GPIO16"},
	{6, 0, "GPIO6"},
	{36, 0, "GPIO36"},
	{0, 0, "GPIO0"},
	{18, 0, "GPIO18"},
	{1, 0, "GPIO1"},
	{20, 0, "GPIO20"},
	{38, 0, "GPIO38"},
	{40, 0, "GPIO40"},
	{4, 0, "GPIO4"},
	{22, 0, "GPIO22"},
	{10, 0, "GPIO10"},
	{26, 0, "GPIO26"},
	{5, 0, "GPIO5"},
	{24, 0, "GPIO24"},
	{15, 0, "GPIO15"},
	{42, 0, "GPIO42"},
	{7, 0, "GPIO7"},
	{30, 0, "GPIO30"},	
};

struct User_Input{
	int Pin_Intensity;
	int Pin_Red;
	int Pin_Green;
	int Pin_Blue;
}Input;

static struct hrtimer hr_timer;

int p_r, d_r, m1_r, m2_r;
int p_g, d_g, m1_g, m2_g;
int p_b, d_b, m1_b, m2_b;

int Global_Red=0, Global_Blue=0, Global_Green=0;
unsigned long ON_time, OFF_time;
int static ON_flag = 0;
ktime_t ON_Interval, OFF_Interval;
ktime_t ktime;
int onlyonce = 1;

/* per device structure */
struct led_dev {
	struct cdev cdev;               /* The cdev structure */
	char name[20];                  /* Name of device*/
	char in_string[256];			/* buffer for the input string */
	int current_write_pointer;
} *led_devp;

static dev_t led_dev_number;      /* Allotted device number */
struct class *led_dev_class;          /* Tie with the device model */
static struct device *led_dev_device;

#define MS_TO_NS(x) (x * 1E6L)

/*
 * Callback function for Kernel HRT Timer
 * Description : This function is responsible for turning ON/OFF the GPIO pins based on sequence
 */
enum hrtimer_restart PWM_func( struct hrtimer *timer )
{
	ktime_t now=ktime_get();
	ktime_t advance;
	/* The callback function alternates between the ON and OFF duration */
	
	if(ON_flag == 0)
		{/* PWM ON Duration */
		/* Set the GPIO Pin Values based on sequence */
		gpio_set_value_cansleep(p_r, (int)Global_Red); 
		gpio_set_value_cansleep(p_b, (int)Global_Blue);
		gpio_set_value_cansleep(p_g, (int)Global_Green); 
		ON_flag = 1;
		advance =  ON_Interval ;
		/* Set the timer to expire after ON time */
		hrtimer_forward(&hr_timer,now,advance);

		}
	else
		{/* PWM OFF Duration */

		/* Set the GPIO Pin Values based on sequence */
		gpio_set_value_cansleep(p_r, 0);
		gpio_set_value_cansleep(p_b, 0);
		gpio_set_value_cansleep(p_g, 0);
	    ON_flag = 0;
	    advance =  OFF_Interval ;
	    /* Set the timer to expire after OFF time */
		hrtimer_forward(&hr_timer,now,advance);
		}
		
		return HRTIMER_RESTART;
}

/*
 * Open led driver
 * Description : This function is responsible for opening the LED driver
 */
int led_driver_open(struct inode *inode, struct file *file)
{
	struct led_dev *led_devp;
	/* Get the per-device structure that contains this cdev */
	led_devp = container_of(inode->i_cdev, struct led_dev, cdev);
	/* Easy access to cmos_devp from rest of the entry points */
	file->private_data = led_devp;
	printk("\n%s is openning \n", led_devp->name);
	return 0;
}

/*
 * Release led driver
 * Description : This function is responsible for closing the LED driver
 */
int led_driver_release(struct inode *inode, struct file *file)
{
	struct led_dev *led_devp = file->private_data;
	
	hrtimer_cancel( &hr_timer );
	
	printk("\n%s is closing\n", led_devp->name);
	
	return 0;
}

/*
 * Write to led driver
 * Description : This function is responsible for extracting the sequence information and updating 
 * global RGB flags
 */
ssize_t led_driver_write(struct file *file, const char *buf,
            size_t count, loff_t *ppos)
 {
	
    int* k_value = (int*)kmalloc(sizeof(int),GFP_KERNEL);
	
	/* Copy the LED sequence from user to kernel space */
	copy_from_user(k_value, (int *)buf, sizeof(int));
	printk(KERN_DEBUG"Ready to write k is %d\n",*k_value);
	
	/* 0 R G B 
	 * Red - 0RGB >> 2 bits
	 * Green - 0RGB >> 1 bit
	 * Blue - 0RGB - no need to shift */
	/* Extract the RGB values from the integer passed */
	Global_Red = (0x04 & *k_value)>>2u;
	
	Global_Blue = (0x01) & *k_value;
	
	Global_Green = (0x02 & *k_value)>>1u;

	if(onlyonce == 1)
	{
	onlyonce =0;
	/* Start the timer once 
	 * INFO : This will be executed only once per power on cycle followed by timer running continuously in BG*/
	ktime = ktime_set(0, MS_TO_NS(10));
	hrtimer_start(&hr_timer, ktime, HRTIMER_MODE_REL);
	}
	
	return 0;
}

/*
 * IO control
 * Description : This function is responsible for configuration of the GPIO pins 
 */
long led_driver_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct User_Input* I;
	
  unsigned long delay_in_ms = 0L;
	
	I = (struct User_Input*)kmalloc(sizeof(struct User_Input),GFP_KERNEL);
	printk(KERN_DEBUG"Ready to configure");

	/* Initialize the HRT timer */
	printk(KERN_INFO"HR Timer module installing\n");
	ktime = ktime_set( 0, MS_TO_NS(delay_in_ms) );
	hrtimer_init( &hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL ); 
	hr_timer.function = &PWM_func;
	printk(KERN_INFO"Starting timer to fire in %ldms (%ld)\n", delay_in_ms, jiffies );	
	onlyonce = 1;
 	
	switch(cmd)
	{
		/* TODO: Check for valid input values else return ERRNO */
		case CONFIG:
		printk(KERN_INFO"Ready to configure");
		/* Retrive the GPIO pin inputs from user space */
		copy_from_user(&I, (struct User_Input*) arg, sizeof(struct User_Input*));
		printk(KERN_DEBUG"Copy done");

		/* Calculate the ON/OFF duration as per the Intensity Value passed by the user */
		ON_time=(I->Pin_Intensity);
		
		/* Calculate the ON_interval and OFF interal for PWM */
		ON_Interval = ktime_set(0,((unsigned long)ON_time *20000000uL)/100);
		OFF_Interval = ktime_set(0,((100 - (unsigned long)ON_time) *20000000uL)/100);

		
		if(((I->Pin_Red<0)||(I->Pin_Red>13))||((I->Pin_Green<0)||(I->Pin_Green>13))||((I->Pin_Blue<0)||(I->Pin_Blue>13)))
		{/* Check for valid pin IO inputs */
			return -1;
		}
		/*******************************/
		/* Set the Pin, Mux and Direction register for Red LED */
		p_r = GpioPin[I->Pin_Red].Pin;
		m1_r = GpioPin[I->Pin_Red].Mux1;
		m2_r = GpioPin[I->Pin_Red].Mux2;
		d_r = GpioPin[I->Pin_Red].Dir;
		
		/*******************************/
		/* Extract the Pin, Mux and Direction for Green LED */
		p_g = GpioPin[I->Pin_Green].Pin;
		m1_g = GpioPin[I->Pin_Green].Mux1;
		m2_g = GpioPin[I->Pin_Green].Mux2;
		d_g = GpioPin[I->Pin_Green].Dir;

	 	/*******************************/
	 	/* Extract the Pin, Mux and Direction for BLUE LED */
		p_b = GpioPin[I->Pin_Blue].Pin;
		m1_b = GpioPin[I->Pin_Blue].Mux1;
		m2_b = GpioPin[I->Pin_Blue].Mux2;
		d_b = GpioPin[I->Pin_Blue].Dir;
		/*******************************/

		printk(KERN_DEBUG"Mux settings");
		gpio_request_array(request_gpio_arr,ARRAY_SIZE(request_gpio_arr));

		/*********************************/
		/* Set the RED led direction to Output */
	 	gpio_direction_output(p_r, 0);

		/*********************************/
	 	/* Set the GREEN led direction to Output */
	 	gpio_direction_output(p_g, 0);

	 	/*********************************/
		/* Set the BLUE led direction to Output */
		gpio_direction_output(p_b, 0);
		break;
		
		default:
		/* Do nothing */
		break;
	}

	return 0;
}


/* File operations structure. Defined in linux/fs.h */
static struct file_operations led_fops = {
    .owner		= THIS_MODULE,             /* Owner */
    .open		= led_driver_open,         /* Open method */
    .release	= led_driver_release,      /* Release method */
    .unlocked_ioctl		= led_driver_ioctl,/* IOCTL method */
    .write		= led_driver_write,        /* Write method */
};

/*
 * Driver Initialization
 */
int __init led_driver_init(void)
{
	int ret;
	/* pre-defined kernel variable jiffies gives current value of ticks */
        /* jiffies to milliseconds, HZ gives number of ticks per second */
	 
	
	/* Request dynamic allocation of a device major number */
	if (alloc_chrdev_region(&led_dev_number, 0, 1, DEVICE_NAME) < 0) {
			printk(KERN_DEBUG "Can't register device\n"); return -1;
	}

	/* Populate sysfs entries */
	led_dev_class = class_create(THIS_MODULE, DEVICE_NAME);

	/* Allocate memory for the per-device structure */
	led_devp = kmalloc(sizeof(struct led_dev), GFP_KERNEL);
	if (!led_devp) {
		printk("Bad Kmalloc\n"); return -ENOMEM;
	}

	/* Request I/O region */
	sprintf(led_devp->name, DEVICE_NAME);

	/* Connect the file operations with the cdev */
	cdev_init(&led_devp->cdev, &led_fops);
	led_devp->cdev.owner = THIS_MODULE;

	/* Connect the major/minor number to the cdev */
	ret = cdev_add(&led_devp->cdev, (led_dev_number), 1);

	if (ret) {
		printk("Bad cdev\n");
		return ret;
	}

	/* Send uevents to udev, so it'll create /dev nodes */
	led_dev_device = device_create(led_dev_class, NULL, MKDEV(MAJOR(led_dev_number), 0), NULL, DEVICE_NAME);	

	

    printk (KERN_INFO "Timer added \n");
    printk("led driver initialized \n");
	return 0;
}

/* 
 *Driver Exit 
 */
void __exit led_driver_exit(void)
{
	/* Release the major number */
	unregister_chrdev_region((led_dev_number), 1);

	/* Destroy device */
	device_destroy(led_dev_class, MKDEV(MAJOR(led_dev_number), 0));
	cdev_del(&led_devp->cdev);
	kfree(led_devp);
	
	hrtimer_cancel( &hr_timer );
	/* Destroy driver_class */
	class_destroy(led_dev_class);
	printk("Timer removed\n");
	printk("led driver removed\n");
}

module_init(led_driver_init);
module_exit(led_driver_exit);
MODULE_LICENSE("GPL v2");



