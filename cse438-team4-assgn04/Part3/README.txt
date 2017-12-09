/* Course : CSE 438 - Embedded Systems Programming
 * Assignment 4 : To implement Signal Handling
 * Team Member1 : Samruddhi Joshi  ASUID : 1213364722
 * Team Member2 : Varun Joshi 	   ASUID : 1212953337 */


#Download and extract the zip file to a local directory 

#Input Files :
Part3 Folder -- Part3a.c , Part3b.c, lib.c, lib.h


#Task 3
========================================================================
Steps to Execute
-----------------------------
1. Navigate to the folder where files have been extracted.
2. Part3 folder conatins two seperate files  for two sub-tasks.

****Change the mouse event*****************
#define MOUSEFILE "/dev/input/eventX" --- change X as per the mouse event in /dev/input/  
 
3. To execute each subtasks, 
	First execute the command "make all" on terminal
	Output file generated for subtask1: "Part3a"
	Output file generated for subtask2: "Part3b"
	
4. To Run the binary
	For Part3 subtask1 : sudo ./Part3a
	For Part3 subtask2 : sudo ./Part3b

#Explanation 
========================================================================
1. Part 3a - Three threads are created which are waiting for signal. On 
first Right click, arbitarily one thread becomes active. On second click, 
any second thread becomes active. On third click, the remaining thread
becomes active. On fourth click, the application exits.

2. Part 3b - 6 threads are created and randomly registered in sigMASK array. 
On detecting right click event, the registered threads become active all 
at once.


	

