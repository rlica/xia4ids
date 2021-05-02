#!/bin/bash

#Author: R. Lica

#This script is used ONLY to monitor NUTAQ using the 'yield' or 'ratemeter' commands
#It opens a run, records for a certain amount of time, and then closes the file,
#doesn't increment the run number, and starts another file with the same number
#to overwrite the previous one so that it doesn't use disk space 


nutaq status >status.txt
RUN_NO=$(awk 'NR==3{ print $2 }' status.txt)
rm status.txt

on_die()
{
	# print message
	#
	
	echo "Dying..."
	nutaq stop
	nutaq setrun $RUN_NO
	
	# Need to exit the script explicitly when done.
	# Otherwise the script would live on, until system
	# realy goes down, and KILL signals are send.
	#
	exit 0
}


# Execute function on_die() receiving TERM signal
#
trap 'on_die' SIGTERM SIGINT


# Loop forever, until the trap activates
while true ; do
	nutaq start
	sleep 1000
	nutaq stop
	nutaq setrun $RUN_NO
done

# We never get here.
exit 0 
