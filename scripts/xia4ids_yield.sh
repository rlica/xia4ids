#!/bin/bash


#R.Lica
##############################


NUMBER_OF_BLOCKS=50           
WRITE_ROOT='y'

RAW_PATH='/home/TapeData/IS608/'
EXP_PATH='/home/idsuser/Experiments/2017/IS608/root/'
CONFIG_FILE='/home/idsuser/Experiments/2017/IS608/root/conf/config_rate'   
CAL_FILE='/home/idsuser/Experiments/2017/IS608/root/conf/EClovers_152Eu.cal'   
ROOT_MACRO='/home/idsuser/Experiments/2017/IS608/root/macros/yield.C'   

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

n4i $CONFIG_FILE $INPUT_FILE $CAL_FILE << echo
$NUMBER_OF_BLOCKS     
$WRITE_ROOT
echo

rm -f n4i_Run0.LOG

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


#bash handles only integer maths but you can use bc
#if (( $(echo "$YIELD511 > 0" |bc -l)  )); then


	#SENDING DATA TO GRAFANA
	#curl --user is633:xNnZcCXr3PJE -i -XPOST 'http://influx.kern.phys.au.dk/write?db=is633'	--data-binary "nutaq,E=511 count=$YIELD511" >> /dev/null 
	curl --user ids:yrYm3HDLCfVN -i -XPOST 'http://influx.kern.phys.au.dk/write?db=ids' --data-binary "IS608 clov_511=$YIELD511" #>> /dev/null 
	curl --user ids:yrYm3HDLCfVN -i -XPOST 'http://influx.kern.phys.au.dk/write?db=ids' --data-binary "IS608 clov_1461=$YIELD1461" #>> /dev/null 


#	fi
	


printf "$(date)    $(du -h $INPUT_FILE) \n"
OLD_FILE_SIZE=$NEW_FILE_SIZE
sleep 5

    	
   else

printf "$(date)    Waiting for file update ... \r"
sleep 1

 fi
 

done
