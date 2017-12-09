/* Course : CSE 438 - Embedded Systems Programming
 * Assignment 4 : To implement Signal Handling
 * Team Member1 : Samruddhi Joshi  ASUID : 1213364722
 * Team Member2 : Varun Joshi 	   ASUID : 1212953337 */


#Download and extract the zip file to a local directory 

#Input Files :
Part2 Folder -- Part2a.c, Part2b.c, Part2c.c


#Task 2
========================================================================
Steps to Execute
-----------------------------
1. Navigate to the folder where files have been extracted.
2. Part2 folder contains three seperate files for three sub-tasks.
3. Configure mouse event for 2b and 2c as per target machine.

****Change the mouse event*****************
#define MOUSEFILE "/dev/input/eventX" --- change X as per the mouse event in /dev/input/ 

3. To execute each subtasks, 
	First execute the command "make all" on terminal
	Output file generated for subtask1: "Part2a"
	Output file generated for subtask2: "Part2b"
	Output file generated for subtask3: "Part2c"
	
4. To execute the binary enter the following command on terminal for respective subtasks:
	For Part2 subtask1 : sudo taskset 0x01 Part2a
	For Part2 subtask2 : sudo taskset 0x01 Part2b
	For Part2 subtask3 : sudo taskset 0x01 Part2c

5. To collect kernel shark details :
	First collect the trace using the command :
	For Part2 subtask1 : sudo trace-cmd record -e sched_switch -e signal taskset 0x01 Part2a	
	For Part2 subtask2 : sudo trace-cmd record -e sched_switch -e signal taskset 0x01 Part2b
	For Part2 subtask3 : sudo trace-cmd record -e sched_switch -e signal taskset 0x01 Part2c
	
	"trace.dat" output file will be generated (after execution of above command-for each subtask)
	
	then for each subtask, execute "kernelshark trace.dat" command on terminal to see kernel shark output

#Explanation 
========================================================================
1. Part 2a - The Higher priority thread generates a signal and continues 
execution and lower priority thread is in runnable state. Only when it 
terminates the signal gets delivered and signal handler is invoked followed
by execution of lower priority thread.

2. Part 2b - The Dummy thread is waiting for semaphore and is in blocked
state. When the user does right click event, the signal is generated and 
signal handler is invoked which in turn resumes thread execution.

3. Part 2c - The Dummy thread is delayed due to nanosleep. If the user does
right click event (<10 sec) then the signal is generated and signal handler
is invoked which in turn halts nanosleep and remaining time is displayed.

Please refer Report.pdf for kernelshark snapshots.

	

