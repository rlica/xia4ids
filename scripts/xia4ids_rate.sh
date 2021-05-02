#!/bin/bash



#R.Lica
##############################

NUMBER_OF_BLOCKS=50           
WRITE_ROOT='n'

RAW_PATH='/home/TapeData/IS608/'
CONFIG_FILE='/home/idsuser/Experiments/2017/IS608/root/conf/config_rate'   


##############################

OLD_FILE_SIZE=0              

while [ 1 = 1 ] ; do

 INPUT_FILE="$(ls -t $RAW_PATH* | head -1)"
 NEW_FILE_SIZE=$(stat -c%s "$INPUT_FILE")


 if [ $NEW_FILE_SIZE -ne $OLD_FILE_SIZE ];

  then
n4i $CONFIG_FILE $INPUT_FILE << echo
$NUMBER_OF_BLOCKS     
$WRITE_ROOT
echo

rm -f n4i_Run0.LOG

    printf "$(date)    $(du -h $INPUT_FILE) \n"
    sleep 3
    OLD_FILE_SIZE=$NEW_FILE_SIZE
    	
   else
    printf "$(date)    Waiting for file update ... \r"
    sleep 1
 fi

done
