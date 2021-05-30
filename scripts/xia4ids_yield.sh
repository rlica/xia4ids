#!/bin/bash



#R.Lica
##############################

OLD_FILE_SIZE=0       

RAW_PATH="$CURRENT_EXP/raw/"
EXP_PATH="$CURRENT_EXP/online/root/"
CONFIG_FILE="$CURRENT_EXP/online/root/conf/config_rate"   
CAL_FILE="$CURRENT_EXP/online/root/conf/EClovers_152Eu.cal"   
ROOT_MACRO="$CURRENT_EXP/online/root/macros/yield.C"   

DATABASE='https://dbod-ids-db.cern.ch:8080/write?db=ids'
LOGIN='admin'
PASSWORD='hello_is_it_rates_youre_looking_for'
TABLE_NAME='pixie'


echo $RAW_PATH 

##############################

OLD_FILE_SIZE=0              


while [ 1 = 1 ] ; do

 INPUT_FILE="$(ls -t $RAW_PATH* | head -1)"
 NEW_FILE_SIZE=$(stat -c%s "$INPUT_FILE")


 if [ $NEW_FILE_SIZE -ne $OLD_FILE_SIZE ];

  then

	#SORTING DATA
	cd $EXP_PATH
	
	xia4ids $CONFIG_FILE $INPUT_FILE $CAL_FILE 
	rm -f log_Run0.txt
	
	sleep 2
	
	#RUNNING ROOT MACRO
	root -b -l -q $ROOT_MACRO"(0)"
	
	
	#EXTRACTING the YIELD 
	YIELD511=$(awk '{print $1}' "$EXP_PATH/out0.txt")
	YIELD1461=$(awk '{print $2}' "$EXP_PATH/out0.txt")
	
	echo "============================================"
	tput setaf 2; echo " Yield of 511 keV (cps) =   $YIELD511"
	tput setaf 2; echo " Yield of 1461 keV (cps) =   $YIELD1461"
	tput sgr0
	echo "============================================"
	
	
	#SENDING DATA TO GRAFANA
	curl --user $LOGIN:$PASSWORD -i -XPOST $DATABASE --data-binary "$TABLE_NAME clov_511=$YIELD511" #>> /dev/null 
	curl --user $LOGIN:$PASSWORD -i -XPOST $DATABASE --data-binary "$TABLE_NAME clov_1461=$YIELD1461" #>> /dev/null 
	
	printf "$(date)    $(du -h $INPUT_FILE) \n"
	OLD_FILE_SIZE=$NEW_FILE_SIZE
	sleep 5

    	
   else

	printf "$(date)    Waiting for file update ... \r"
	sleep 1

 fi
 

done
