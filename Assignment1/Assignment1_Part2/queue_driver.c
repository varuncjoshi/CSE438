/* Course : CSE 438 - Embedded Systems Programming
 * Assignment 1b : To implement Shared Message queues in Kernel Space
 * Team Member1 : Samruddhi Joshi  ASUID : 1213364722
 * Team Member2 : Varun Joshi 	   ASUID : 1212953337 */

/* ----------------------------------------------- DRIVER queue --------------------------------------------------
 
 Device driver to implement shared queues in kernel space
 
 ----------------------------------------------------------------------------------------------------------------*/
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

#define DEVICE_NAME   "QUEUE_DRIVER"  // device name to be created and registered

#define QUEUE_LEN 10

 static __inline__ unsigned long long my_rdtsc(void)
 {
     unsigned long lo, hi;
     __asm__ __volatile__ ( "rdtsc" : "=a" (lo), "=d" (hi) ); 
     return( (unsigned long long)lo | ((unsigned long long)hi << 32) );
 }

int queue_open(struct inode *inode, struct file *file);

static dev_t queue_id;

struct message 
{
	int message_id;
	int source_id;
	unsigned long long queue_time; //queueing time/start time
	double pi_value;
};

/* Structure for DataQueue */
struct dataqueue {
	int front;
	int rear;
	int full;
	struct message* message_pointer[QUEUE_LEN];
};

/* per device structure */
static struct queue_dev {
	struct cdev cdev;               /* The cdev structure */
	struct dataqueue queue; 
	char *name;               /* Name of device*/		
	struct mutex kernel_mutex;
}*queue_dev[2];

      /* Allotted device number */
struct class *queue_dev_class;          /* Tie with the device model */


/*
* Open queue driver
*/
int queue_open(struct inode *inode, struct file *file)
{
	struct queue_dev *queue_devp;

	/* Get the per-device structure that contains this cdev */
	queue_devp = container_of(inode->i_cdev, struct queue_dev, cdev);
	/* Easy access to cmos_devp from rest of the entry points */
	file->private_data = queue_devp;
	printk(KERN_INFO"\n%s is openning \n", queue_devp->name);
	return 0;
}

/*
 * Release queue driver
 */
int queue_release(struct inode *inode, struct file *file)
{
	struct queue_dev *queue_devp = file->private_data;
	
	printk(KERN_INFO"\n%s is closing\n", queue_devp->name);
	return 0;
}

/*
 * Write to queue driver
 */
ssize_t queue_write(struct file *file, const char* msg,
           size_t count, loff_t *ppos)
{
	struct queue_dev *queue_devp = file->private_data;
	 
	
	/* Check whether Data Queue is full */
	if((queue_devp->queue.rear==queue_devp->queue.front) && (queue_devp->queue.full == 1))
		{
		printk(KERN_INFO"Queue of %s is full\n",queue_devp->name);
		return -1; /* Cannot write into queue */
		}
	mutex_lock(&queue_devp->kernel_mutex);

	queue_devp->queue.message_pointer[queue_devp->queue.front]= (struct message*)kmalloc(sizeof(struct message),GFP_KERNEL);
	
	if(copy_from_user((queue_devp->queue.message_pointer[queue_devp->queue.front]),(struct message*)msg,sizeof(struct message)))
		printk(KERN_DEBUG"Cannot write in %s\n",queue_devp->name);
	/* Capture Enqueue time */
	queue_devp->queue.message_pointer[queue_devp->queue.front]->queue_time = my_rdtsc();
	/* Increment the front to next write location */	
	queue_devp->queue.front++; 
	/* To make the buffer circular */
	queue_devp->queue.front = queue_devp->queue.front % QUEUE_LEN;
	if(queue_devp->queue.rear==queue_devp->queue.front)
		{queue_devp->queue.full  = 1;} /* Set flag as buffer is full */

	mutex_unlock(&queue_devp->kernel_mutex);
	printk(KERN_INFO"Writing to the Queue %s done\n",queue_devp->name);
	return 0;
}
/*
 * Read to queue driver
 */
ssize_t queue_read(struct file *file, char* msg,
           size_t count, loff_t *ppos)
{
	int index;
	int data;
	struct queue_dev *queue_devp = file->private_data;
	unsigned long long current_time; 
	
   	/* Check whether Data Queue is empty */
	if((queue_devp->queue.rear==queue_devp->queue.front) && !(queue_devp->queue.full))
		{
		//Nothing to read
		return -1;
		}
	
	mutex_lock(&queue_devp->kernel_mutex);
	index = queue_devp->queue.rear;
	/* Calculate the dequeue time = Current time - Enqueued time */
	current_time = my_rdtsc();
	queue_devp->queue.message_pointer[index]->queue_time = (current_time - (queue_devp->queue.message_pointer[index]->queue_time));
	data = copy_to_user((struct message*)msg,(queue_devp->queue.message_pointer[index]),sizeof(struct message));
	if(data)
		printk(KERN_INFO"Cannot read from queue");
	kfree(queue_devp->queue.message_pointer[index]);
	/* Increment the front to next read location */
	queue_devp->queue.rear++;
	/* To make the buffer circular */
	queue_devp->queue.rear = queue_devp->queue.rear % QUEUE_LEN;
	if(queue_devp->queue.rear == queue_devp->queue.front)
		queue_devp->queue.full = 0; /* Set flag as buffer is empty */
	
	mutex_unlock(&queue_devp->kernel_mutex);
	printk(KERN_INFO"Reading from the Queue %s done\n",queue_devp->name);
	return 0;

}

/* File operations structure. Defined in linux/fs.h */
static struct file_operations queue_fops = {
    .owner		= THIS_MODULE,           /* Owner */
    .open		= queue_open,        /* Open method */
    .release	= queue_release,     /* Release method */
    .write		= queue_write,       /* Write method */
    .read		= queue_read,        /* Read method */
};

/*
 * Driver Initialization
 */
int __init queue_driver_init(void)
{
	int ret,i;

	/* Request dynamic allocation of a device major number */
	if (alloc_chrdev_region(&queue_id, 0, 2, DEVICE_NAME) < 0) {
			printk(KERN_DEBUG"Can't register device\n"); 
			return -1;
	}

	/* Populate sysfs entries */
	queue_dev_class = class_create(THIS_MODULE, DEVICE_NAME);

	/* Allocate memory for the per-device structure */
	for(i=0;i<2;i++)
	{
		queue_dev[i] = (struct queue_dev*)kmalloc(sizeof(struct queue_dev), GFP_KERNEL);
		if (!queue_dev[i]) {
		printk("Bad Kmalloc\n"); 
		return -ENOMEM;
		}
		queue_dev[i]->queue.rear = 0;
		queue_dev[i]->queue.front = 0;
		queue_dev[i]->queue.full = 0;
		mutex_init(&(queue_dev[i]->kernel_mutex));
	}

	queue_dev[0]->name = "DataQueue1";
	queue_dev[1]->name = "DataQueue2";

	for(i=0;i<2;i++)
	{
		/* Send uevents to udev, so it'll create /dev nodes */
		// device_create_file(queue_dev_device, &dev_attr_xxx);	
		if((device_create(queue_dev_class, NULL, MKDEV(MAJOR(queue_id), MINOR(queue_id+i)), NULL, "DataQueue%d",i+1)==NULL))
		{
			class_destroy(queue_dev_class);
			unregister_chrdev_region(queue_id,2);
			printk(KERN_DEBUG"Cannot register device");
		}

		/* Connect the file operations with the cdev */
		cdev_init(&queue_dev[i]->cdev,&queue_fops);
		queue_dev[i]->cdev.owner = THIS_MODULE;
		/* Connect the major/minor number to the cdev */
		ret = cdev_add(&queue_dev[i]->cdev, MKDEV(MAJOR(queue_id), MINOR(queue_id+i)), 1);
		if (ret) {
		printk("Bad cdev\n");
		return ret;
		}
	}
	
	printk(KERN_INFO"DataQueue Devices created\n");
	return 0;
}


/* Driver Exit */
void __exit queue_driver_exit(void)
{
	int i;
	// device_remove_file(queue_dev_device, &dev_attr_xxx);
	/* Release the major number */
	for(i=0;i<2;i++)
	{
		/* Destroy device */
		device_destroy (queue_dev_class, MKDEV(MAJOR(queue_id), MINOR(queue_id+1)));
		/* Delete cdev entries */
		cdev_del(&queue_dev[i]->cdev);
		kfree(queue_dev[i]);
	}
		/* Destroy driver_class */
	class_destroy(queue_dev_class);
	unregister_chrdev_region(queue_id,2);

	printk(KERN_INFO"Queue driver removed.\n");
}

module_init(queue_driver_init);
module_exit(queue_driver_exit);
MODULE_LICENSE("GPL v2");
