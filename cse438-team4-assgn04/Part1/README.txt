/* Course : CSE 438 - Embedded Systems Programming
 * Assignment 4 : To implement Signal Handling
 * Team Member1 : Samruddhi Joshi  ASUID : 1213364722
 * Team Member2 : Varun Joshi 	   ASUID : 1212953337 */


#Download and extract the zip file to a local directory 

#Input Files :
Part1 Folder -- Part1.c


#Task 1
========================================================================
Steps to Execute
-----------------------------
1. Navigate to the folder where files have been extracted.
2. Part 1 files are present in Part1 folder.

****Change the mouse event*****************
#define MOUSEFILE "/dev/input/eventX" --- change X as per the mouse event in /dev/input/ 

3. Run the command in terminal "make all"
    Output file generated : "Part1"
4. To exceute the binary, enter the command on terminal :
	 sudo ./Part1

#Explanation 
========================================================================
Part1 makes use of two threads- One thread is used to generate the signal
by detecting double right click from the mouse and second thread is used
to do computation. The results of the imprecise computation is displayed 
as soon as double click right event is detected. For double click event,
the thread waits for 300ms for second click otherwise it is detected as 
a first click. The MOUSE DEVICE path can be changed to the 
#define MOUSEFILE "/dev/input/event4"to correct valueas per the settings
of the tester machine.

	

