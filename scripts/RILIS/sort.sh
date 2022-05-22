#!/bin/bash


### The next two lines are needed for running this script via ssh on the pixie16@pixieids
export PATH=$PATH:/opt/root/6.18.04/bin/
export LD_LIBRARY_PATH=/opt/root/6.18.04/lib
###



RAW_PATH="${CURRENT_EXP}/RAW/"
EXP_PATH="${CURRENT_EXP}/ROOT/"
CAL_FILE="${CURRENT_EXP}/ROOT/cal/online_low.cal"
CONF_FILE="${CURRENT_EXP}/ROOT/conf/config_root_online"



#REC_TIME=20
#Start/stop nutaq 
#nutaq start
#sleep $REC_TIME
#nutaq stop 

#Removing previous result files
rm /home/pixie16/RILIS/out.txt
rm /home/pixie16/RILIS/spec.a8

#Reading the most recent file number - format runXYZ.ldf
cd $RAW_PATH
RUN_NO="$(ls -1t | head -1 | sed 's/.ldf//' | sed 's/run_//' | sed 's/^0*//')"
echo "Sorting Run $RUN_NO"


######################################################### To make the script faster, comment from here

#Sorting RAW data into ROOT Format
cd $EXP_PATH
xia4ids $CONF_FILE $CAL_FILE << echo
$RUN_NO     
$RUN_NO
echo


#Extracting info from the ROOT tree
root -b -l -q macros/yield.C"($RUN_NO)"
root -b -l -q macros/root2spec.C"($RUN_NO)"

#Saving results in the RILIS folder
mv out$RUN_NO.txt /home/pixie16/RILIS/out.txt
mv spec$RUN_NO.a8 /home/pixie16/RILIS/spec.a8
#
######################################################## To here

####################################################### And uncomment the following lines
#
#echo "$RUN_NO   -1   -1" > /home/idsuser/RILIS/out.txt

