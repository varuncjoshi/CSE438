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
#define CONFIG _IOR('I', 1, struct User_Input)


struct User_Input{
	int Pin_Intensity;
	int Pin_Red;
	int Pin_Green;
	int Pin_Blue;
};


#endif /* __GPIO_FUNC_H__ */
