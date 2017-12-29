#!/bin/bash
#  * Course : CSE 438 - Embedded Systems Programming
#  * Assignment 2 : To implement GPIO control 
#  * Team Member1 : Samruddhi Joshi  ASUID : 1213364722
#  * Team Member2 : Varun Joshi 	 ASUID : 1212953337 


Red_Led=$2              
Green_Led=$3
Blue_Led=$4
CYCLE_DURATION=20         #Duty cycle
SEQUENCE_DURATION=500	  	
Intensity_Val=$1			
sequence_count=0
ON=1
OFF=0
DIR=out
Exit_Flag=0

					#Pin Dir Mux1 Mux2
declare -a GpioPin0=( 11 32 0 0 ) #IO0
declare -a GpioPin1=( 12 28 45 0 ) #IO1
declare -a GpioPin2=( 13 34 77 0 ) #IO2	             
declare -a GpioPin3=( 14 16 76 64 ) #IO3
declare -a GpioPin4=( 6 36 0 0 ) #IO4
declare -a GpioPin5=( 0 18 66 0 ) #IO5
declare -a GpioPin6=( 1 20 68 0 ) #IO6
declare -a GpioPin7=( 38 0 0 0 ) #IO7
declare -a GpioPin8=( 40 0 0 0 ) #IO8
declare -a GpioPin9=( 4 22 70 0 ) #IO9
declare -a GpioPin10=( 10 26 74 0 ) #IO10
declare -a GpioPin11=( 5 24 44 72 ) #IO11
declare -a GpioPin12=( 15 42 0 0 ) #IO12
declare -a GpioPin13=( 7 30 46 0 )  #IO13

  

# Select the pins from the array corresponding to the input for Red LED

	p_r=$[GpioPin$Red_Led[0]]
	m1_r=$[GpioPin$Red_Led[2]]
	m2_r=$[GpioPin$Red_Led[3]]
	d_r=$[GpioPin$Red_Led[1]]
		

# Export the required GPIO iff not exported already
		
	 		
		
		if [ ! -d /sys/class/gpio/gpio$m1_r ] ; then
		 	echo -n $m1_r > /sys/class/gpio/export
		fi			
		
		if [ ! -d /sys/class/gpio/gpio$m2_r ] ; then
		 	echo -n $m2_r > /sys/class/gpio/export
		fi

		if [ ! -d /sys/class/gpio/gpio$d_r ] ; then
		 	echo -n $d_r > /sys/class/gpio/export
		fi		
			
		if [ ! -d /sys/class/gpio/gpio$p_r ] ; then
		 	echo -n $p_r > /sys/class/gpio/export
		fi 
	 
	 
#Set the Pin direction for red led


	 	echo -n $DIR > /sys/class/gpio/gpio$d_r/direction
	 	echo -n 0 > /sys/class/gpio/gpio$d_r/value
		echo -n $DIR > /sys/class/gpio/gpio$p_r/direction
	
	

# Select the pins from the array corresponding to the input for Green LED

	p_g=$[GpioPin$Green_Led[0]]
	m1_g=$[GpioPin$Green_Led[2]]
	m2_g=$[GpioPin$Green_Led[3]]
	d_g=$[GpioPin$Green_Led[1]]
	

# Export the required GPIO iff not exported already	
		
	 		
		
		if [ ! -d /sys/class/gpio/gpio$m1_g ] ; then
		 	echo -n $m1_g > /sys/class/gpio/export
		fi			
		
		if [ ! -d /sys/class/gpio/gpio$m2_g ] ; then
		 	echo -n $m2_g > /sys/class/gpio/export
		fi
		
		if [ ! -d /sys/class/gpio/gpio$d_g ] ; then
		 	echo -n $d_g > /sys/class/gpio/export
		fi		

		if [ ! -d /sys/class/gpio/gpio$p_g ] ; then
		 	echo -n $p_g > /sys/class/gpio/export
		fi 
#Set the Pin direction for green led	 

		echo -n $DIR > /sys/class/gpio/gpio$d_g/direction
	 	echo -n 0 > /sys/class/gpio/gpio$d_g/value
		echo -n $DIR > /sys/class/gpio/gpio$p_g/direction



# Select the pins from the array corresponding to the input for Blue LED
	
	p_b=$[GpioPin$Blue_Led[0]]
	m1_b=$[GpioPin$Blue_Led[2]]
	m2_b=$[GpioPin$Blue_Led[3]]
	d_b=$[GpioPin$Blue_Led[1]]
	


# Export the required GPIO iff not exported already		
	 		
		if [ ! -d /sys/class/gpio/gpio$m1_b ] ; then
		 	echo -n $m1_b > /sys/class/gpio/export
		fi			

		if [ ! -d /sys/class/gpio/gpio$d_b ] ; then
		 	echo -n $d_b > /sys/class/gpio/export
		fi		
		
		if [ ! -d /sys/class/gpio/gpio$m2_b ] ; then
		 	echo -n $m2_b > /sys/class/gpio/export
		fi
		
		if [ ! -d /sys/class/gpio/gpio$p_b ] ; then
		 	echo -n $p_b > /sys/class/gpio/export
		fi 
	 
	
#Set the Pin direction for blue led

	 echo -n $DIR > /sys/class/gpio/gpio$d_b/direction
	 echo -n 0 > /sys/class/gpio/gpio$d_b/value
	 echo -n $DIR > /sys/class/gpio/gpio$p_b/direction
	




#Sequence R,G,B,RG,RB,GB,RGB 
while [ $Exit_Flag -lt 1 ]                        #Terminate when there is mouse event detection
do

	on_time=$[$CYCLE_DURATION*$Intensity_Val/100]
	off_time=$[$CYCLE_DURATION-$on_time]
	on_time=$[$on_time*1000]	
	off_time=$[$off_time*1000]
	loop_count=$[$SEQUENCE_DURATION/$CYCLE_DURATION]
	

		case "$sequence_count" in
			

		0)
			
			for((loop_i=0; loop_i<$loop_count; loop_i++))
			do
				echo "Sequence : sequence_R"
				echo -n $ON > /sys/class/gpio/gpio$p_r/value
				usleep $on_time
				echo -n $OFF > /sys/class/gpio/gpio$p_r/value 
				usleep $off_time
				
			done
			sequence_count=1 
			;;
			
		1)
			
			for((loop_i=0; loop_i<$loop_count; loop_i++))
			do
				echo "Sequence : sequence_G"
				echo -n $ON > /sys/class/gpio/gpio$p_g/value
				usleep $on_time
				echo -n $OFF > /sys/class/gpio/gpio$p_g/value 
				usleep $off_time
				
			done
			sequence_count=2
			;;
		2)
		
			
			for((loop_i=0; loop_i<$loop_count; loop_i++))
			do
				echo "Sequence : sequence_B"
				echo -n $ON > /sys/class/gpio/gpio$p_b/value
				usleep $on_time
				echo -n $OFF > /sys/class/gpio/gpio$p_b/value 
				usleep $off_time
				
			done
			sequence_count=3
			;;

		3)

			
			for((loop_i=0; loop_i<$loop_count; loop_i++))
			do
				echo "Sequence : sequence_RG"
				echo -n $ON > /sys/class/gpio/gpio$p_r/value
				echo -n $ON > /sys/class/gpio/gpio$p_g/value
				usleep $on_time
				echo -n $OFF > /sys/class/gpio/gpio$p_r/value
				echo -n $OFF > /sys/class/gpio/gpio$p_g/value  
				usleep $off_time
				
			done
			sequence_count=4
			;;

		4)
		
			
			for((loop_i=0; loop_i<$loop_count; loop_i++))
			do
				echo "Sequence : sequence_RB"
				echo -n $ON > /sys/class/gpio/gpio$p_r/value
				echo -n $ON > /sys/class/gpio/gpio$p_b/value
				usleep $on_time
				echo -n $OFF > /sys/class/gpio/gpio$p_r/value
				echo -n $OFF > /sys/class/gpio/gpio$p_b/value 
				usleep $off_time
				
			done
			sequence_count=5
			;;	


		5)
		
			
			for((loop_i=0; loop_i<$loop_count; loop_i++))
			do
				echo "Sequence : sequence_GB"
				echo -n $ON > /sys/class/gpio/gpio$p_g/value
				echo -n $ON > /sys/class/gpio/gpio$p_b/value
				usleep $on_time
				echo -n $OFF > /sys/class/gpio/gpio$p_g/value
				echo -n $OFF > /sys/class/gpio/gpio$p_b/value 
				usleep $off_time
				
				
			done
			sequence_count=6
			;;
		6)
		
			
			for((loop_i=0; loop_i<$loop_count; loop_i++))
			do
				echo "Sequence : sequence_RGB"
				echo -n $ON > /sys/class/gpio/gpio$p_r/value
				echo -n $ON > /sys/class/gpio/gpio$p_g/value
				echo -n $ON > /sys/class/gpio/gpio$p_b/value
				usleep $on_time
				echo -n $OFF > /sys/class/gpio/gpio$p_r/value
				echo -n $OFF > /sys/class/gpio/gpio$p_g/value 
				echo -n $OFF > /sys/class/gpio/gpio$p_b/value  
				usleep $off_time
				
			done
			sequence_count=0
			;;		

		*)
			break;;
		esac
done

