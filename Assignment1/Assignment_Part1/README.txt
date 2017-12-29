/* Course : CSE 438 - Embedded Systems Programming
 * Assignment 1 : To implement Shared Message queues in User Space
 * Team Member1 : Samruddhi Joshi  ASUID : 1213364722
 * Team Member2 : Varun Joshi 	   ASUID : 1212953337 */

#INPUT FILES#
main.c
queue.c
queue.h
makefile

#OUTPUT FILES#
main.o
queue.o
main
trace.dat

#PRE_REQUISITES#

#Download all the input files from GitHub repository to the a local folder

#Set input event of mouse in the 'main.c' file as per the event detected in "dev/input/" corresponding to "platform-i8042-serio-1-event-mouse"
Example :  "platform-i8042-serio-1-event-mouse-> ../event4"
#define MOUSEFILE "/dev/input/event4"--- change the mouse event here as event4 in the main.c file

#Depending on the linux host, enable/disable the compiler switch for Affinity in main.h
#undef AFFINITY ----> for Virtual Machine & use "sudo TASKSET 0 ./main"
#define AFFINITY -----> for Dual Boot (default setting)

#EXECUTE MAKE COMMAND#

#Change the directory to the local folder where files are present in the terminal using "cd" command

#MAKE -  Execute command "make" on terminal to compile and generate an executable and object files
	 main executable gets generated
	 queue.o main.o object files get generated

#MAKE RUN - Execute command "make run" on terminal to run the executable	

#MAKE TRACE - Exceute the trace command "make trace" --> trace.dat file gets generated

#MAKE KERNELSHARK - Execute the command "make kernelshark" on terminal to view the scheduling of threads on kernelshark application.

#MAKE CLEAN - To clean the output files
		 
