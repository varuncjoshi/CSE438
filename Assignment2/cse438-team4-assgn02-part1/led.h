/* Course : CSE 438 - Embedded Systems Programming
 * Assignment 2 : To implement GPIO control in linux
 * Team Member1 : Samruddhi Joshi  ASUID : 1213364722
 * Team Member2 : Varun Joshi 	   ASUID : 1212953337 
*/


#ifndef __GPIO_FUNC_H__


 /****************************************************************
 * Constants
 ****************************************************************/
 
#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 64

/****************************************************************
 * Functions
 ****************************************************************/

int gpio_export(unsigned int gpio);
int gpio_unexport(unsigned int gpio);
int gpio_set_dir(unsigned int gpio, unsigned int out_flag);
int gpio_set_value(unsigned int gpio, unsigned int value);
int mux_gpio_set(unsigned int gpio, unsigned int value);

#endif /* __GPIO_FUNC_H__ */
