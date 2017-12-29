/* Course : CSE 438 - Embedded Systems Programming
 * Assignment 2 : To implement Shared Message queues in kernel Space
 * Team Member1 : Samruddhi Joshi  ASUID : 1213364722
 * Team Member2 : Varun Joshi 	   ASUID : 1212953337 */

#INPUT FILES#
RGBLed.c
Pin_functions.c
led.h
Makefile

#OUTPUT FILES#
RGBLed

#PRE_REQUISITES#

#Download the zip file from GitHub repository and extract it to a local folder in your host

#Ensure the usb serial cable is recognised, ethernet cable is connected, board is powered up and sd card boot image is inserted and LED pins are connected to Board

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
#In putty terminal set static ip by command -> ifconfig enp0s20f6 192.168.1.5 netmask 255.255.0.0 up


#CHECK MOUSE EVENT#
Connect the usb mouse to galilieo board and run following command to check mouse event number
cat /proc/bus/input/devices | grep mouse
Set input event of mouse in the 'RGBLed.c' file as per the event detected  
Example :  "#define MOUSEFILE "/dev/input/eventx-> x is 3 if event3 is detected"

##COMPILE THE CODE##
#On the host,go to the folder where you copied all the input files 
#To compile for Galileo board use command on terminal -> make TEST_TARGET=Galileo

#COPY FILES TO TARGET#
# Create a folder in target by giving command in putty -> mkdir led
# Copy the files to target by giving command in terminal -> scp <files to copy> root@192.168.1.5:/home/root/led
example : scp RGBLed Pin_Functions.ko root@192.168.1.5:/home/root/queue

##TO INSERT MODULE##
#To insert the module in kernel (target) -> insmod ./Pin_Functions.ko

##TO RUN BINARY##
#To run the binary on target go to the folder where you copied the binary and execute  
for example : /home/root/led -> run the binary RGBLed by entering the command
./RGBLed 50 0 1 2
where the 4 arguments indicate (as per the problem statement) Intensity(in percentage),Pin1(Red),Pin2(Green),Pin3(Blue)
"Intensity values vary between 0 to 100 and pins vary from 0 to 13 except 7,8"    

#MAKE CLEAN - To clean the output files
On host, enter command "make clean" on terminal to remove the output files  
