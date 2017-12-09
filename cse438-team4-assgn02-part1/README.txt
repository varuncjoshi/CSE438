/* Course : CSE 438 - Embedded Systems Programming
 * Assignment 2 : GPIO Control in Linux User Space
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

#Ensure the usb serial cable is recognised, ethernet cable is connected, board is powered up and sd card boot image is inserted

#LED Setup#
#Connect the RGB pins of led with the jumper wires to required input GPIO. Connect GND to GND on target board.

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
Connect the usb mouse to galilieo board and run following command to check mouse event number
cat /proc/bus/input/devices | grep mouse
Set input event of mouse in the 'RGBLed.c' file as per the event detected  
Example :  "#define MOUSEFILE "/dev/input/eventx-> x is 2 if event2 is detected"

COMPILE THE CODE AND EXECUTE THE BINARY
#On the host,go to the folder where you copied all the input files 
#To compile for Galileo board use command on terminal -> make all

#TO RUN BINARY#
Copy the binary file(or executable file) created (after running make all) from host to the target through the command
example : scp RGBLed root@192.168.1.5:/home/root/led

#To run the binary on target go to the folder where you copied the binary and execute  
for example : /home/root/led -> run the binary RGBLed by entering the command
./RGBLed

#MAKE CLEAN - To clean the output files
On host, enter command "make clean" on terminal to remove the output files  
