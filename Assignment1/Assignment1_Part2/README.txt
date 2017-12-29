/* Course : CSE 438 - Embedded Systems Programming
 * Assignment 1 : To implement Shared Message queues in User Space
 * Team Member1 : Samruddhi Joshi  ASUID : 1213364722
 * Team Member2 : Varun Joshi 	   ASUID : 1212953337 */

#INPUT FILES#
main.c
queue_driver.c
queue.h
makefile

#OUTPUT FILES#
main.o
queue_driver.o
queue_tester
queue_driver.ko

#PRE_REQUISITES#
#Download all the input files from GitHub repository to the a local folder

#Depending on the linux host, enable/disable the compiler switch for Affinity in main.h
#undef AFFINITY ----> for Virtual Machine & use "sudo TASKSET 0 ./main" (default)
#define AFFINITY -----> for Dual Boot 

#Ensure the usb serial cable is recognised, ethernet cable is connected, board is powered up and sd card boot image is inserted

#SET UP PUTTY#
#Check the ttyUSB number and enter same below
sudo chmod 777 /dev/ttyUSB#
#Give command -> sudo  putty
#Select serial and set serial line = /dev/ttyUSB# and baudrate(speed) = 115200
example : serial line = /dev/ttyUSB0 and baudrate(speed) = 115200
#Once putty terminal opens press enter->root

#SET STATIC IP#
#HOST
#In terminal check device name by command ifconfig(eg-enp0s3)
#Set static ip by command -> sudo ifconfig enp0s3 192.168.1.2 netmask 255.255.0.0 up

#TARGET
#In terminal check device name by command ifconfig(eg:enp0s20f6)
#In putty terminal set static ip by command -> ifconfig enp0s20f6 192.168.1.2 netmask 255.255.0.0 up

#CHECK MOUSE EVENT#
#Connect the usb mouse to galilieo board and run following command to check mouse event number
cat /proc/bus/input/devices | grep mouse
#Enter this event number in main.c
#To compile for Galileo board use command on terminal -> make TEST_TARGET=Galileo
#To compile for the Linux host use command on terminal -> make TEST_TARGET=host

#COPY FILES TO TARGET#
# Create a folder in target by giving command in putty -> mkdir queue
# Copy the files to target by giving command in terminal -> scp <files to copy> root@192.168.1.5:/home/root/queue
example : scp queue_tester queue_driver.ko root@192.168.1.5:/home/root/queue

#TO INSERT MODULE#
#To insert the module in kernel (target) -> insmod ./queue_driver.ko
#To insert the module in kernel (host) -> sudo insmod ./queue_driver.ko

#TO RUN BINARY#
#To run the binary on target go to the folder /home/root/queue -> run the binary queue_tester
#To run the binary on host go to the folder where files are saved -> run the binary queue_tester

#MAKE CLEAN - To clean the output files
		 
