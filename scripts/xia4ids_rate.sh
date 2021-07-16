#!/bin/bash



#R.Lica
##############################

OLD_FILE_SIZE=0              

while [ 1 = 1 ] ; do

 INPUT_FILE="$(ls -t $CURRENT_EXP/RAW* | head -1)"
 NEW_FILE_SIZE=$(stat -c%s "$CURRENT_EXP/RAW/$INPUT_FILE")


 if [ $NEW_FILE_SIZE -ne $OLD_FILE_SIZE ];

  then
xia4ids config_rate $CURRENT_EXP/RAW/$INPUT_FILE

rm -f n4i_Run0.LOG

    printf "$(date)    $(du -h $CURRENT_EXP/RAW/$INPUT_FILE) \n"
    sleep 10
    OLD_FILE_SIZE=$NEW_FILE_SIZE
    	
   else
    printf "$(date)    Waiting for file update ... \r"
    sleep 1
 fi

done
