#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include "led.h"

/***********************************************************************
* gpio_export - Function to export gpio pins.
* @gpio: GPIO PIN Number
*
* Returns 0 on success.
* 
* Description: Function to export gpio pins.
***********************************************************************/
int gpio_export(unsigned int gpio)
{
	int fd, len;
	char buf[MAX_BUF];
 
	fd = open("/sys/class/gpio/export", O_WRONLY);
	if(fd < 0)
	{
		perror("gpio/export");
		return fd;
	}
 
	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);
 
	return 0;
}

/***********************************************************************
* gpio_export - Function to unexport gpio pins.
* @gpio: GPIO PIN Number
*
* Returns 0 on success.
* 
* Description: Function to unexport gpio pins.
***********************************************************************/
int gpio_unexport(unsigned int gpio)
{
	int fd, len;
	char buf[MAX_BUF];
 
	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if(fd < 0)
	{
		perror("gpio/export");
		return fd;
	}
 
	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);
	return 0;
}

/***********************************************************************
* gpio_set_dir - Function to set directions for gpio pins.
* @gpio: GPIO PIN Number
* @out_flag: Directions of GPIO PIN
*
* Returns 0 on success.
* 
* Description: Function to set directions for gpio pins.
***********************************************************************/
int gpio_set_dir(unsigned int gpio, unsigned int out_flag)
{
	int fd, len;
	char buf[MAX_BUF];
 
	len = snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/direction", gpio);
 
	fd = open(buf, O_WRONLY);
	if(fd < 0)
	{
		perror("gpio/direction");
		return fd;
	}
 
	if (out_flag == 0)
		write(fd, "out", len+1);
	else
		write(fd, "in", len+1);
 
	close(fd);
	return 0;
}

/***********************************************************************
* gpio_set_value - Function to set value for gpio pins.
* @gpio: GPIO PIN Number
* @value: value of GPIO PIN
*
* Returns 0 on success.
* 
* Description: Function to set value for gpio pins.
***********************************************************************/
int gpio_set_value(unsigned int gpio, unsigned int value)
{
	int fd, len;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/value", gpio);

	fd = open(buf, O_WRONLY);
	if(fd < 0)
	{
		perror("gpio/set-value");
		return fd;
	}

	if(value == GPIO_VALUE_HIGH)
		write(fd, "1", len+1);
	else
		write(fd, "0", len+1);

	close(fd);
	return 0;
}


/***********************************************************************
* gpio_set_edge - Function to set edge for gpio pins.
* @gpio: GPIO PIN Number
* @edge: edge of GPIO PIN
*
* Returns 0 on success.
* 
* Description: Function to set edge for gpio pins.
***********************************************************************/
int gpio_set_edge(unsigned int gpio, char *edge)
{
	int fd, len;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/edge", gpio);
 
	fd = open(buf, O_WRONLY);
	if(fd < 0)
	{
		perror("gpio/set-edge");
		return fd;
	}
 
	write(fd, edge, len+1); 
	close(fd);
	return 0;
}


/***********************************************************************
* gpio_fd_open - Function to close file descriptor for the gpio.
* @gpio: GPIO PIN Number
*
* Returns 0 on success.
* 
* Description: Function to close file descriptor for the gpio.
***********************************************************************/
int gpio_fd_close(int fd)
{
	return close(fd);
}
