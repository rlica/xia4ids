#!/bin/bash



#R.Lica
##############################

NUMBER_OF_BLOCKS=50           
WRITE_ROOT='n'

RAW_PATH='/home/TapeData/IS641/IS641/'
CONFIG_FILE='/home/idsuser/Experiments/2018/IS641/root/conf/config_rate'   
EXPERIMENT='IS610'

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

###Send rates to grafana

TOTAL_LINES=$(cat n4i_Run0.LOG | wc -l)

for ((LINE=3; LINE<=TOTAL_LINES; LINE++)); 

do


TOTAL=$(awk -v i=$LINE 'NR==i { print $3 }' "n4i_Run0.LOG")
FAIL=$(awk -v i=$LINE 'NR==i { print $4 }' "n4i_Run0.LOG")
PILEUP=$(awk -v i=$LINE 'NR==i { print $5 }' "n4i_Run0.LOG")
RATE=$(awk -v i=$LINE 'NR==i { print $6 }' "n4i_Run0.LOG")

curl --user ids:yrYm3HDLCfVN -i -XPOST 'http://influx.kern.phys.au.dk/write?db=ids' --data-binary "$EXPERIMENT  TOTAL_$LINE=$TOTAL" #>> /dev/null 
curl --user ids:yrYm3HDLCfVN -i -XPOST 'http://influx.kern.phys.au.dk/write?db=ids' --data-binary "$EXPERIMENT   FAIL_$LINE=$FAIL" #>> /dev/null 
curl --user ids:yrYm3HDLCfVN -i -XPOST 'http://influx.kern.phys.au.dk/write?db=ids' --data-binary "$EXPERIMENT PILEUP_$LINE=$PILEUP" #>> /dev/null 
curl --user ids:yrYm3HDLCfVN -i -XPOST 'http://influx.kern.phys.au.dk/write?db=ids' --data-binary "$EXPERIMENT   RATE_$LINE=$RATE" #>> /dev/null 
	
done

rm -f n4i_Run0.LOG

    printf "$(date)    $(du -h $INPUT_FILE) \n"
    sleep 3
    OLD_FILE_SIZE=$NEW_FILE_SIZE
    	
   else
    printf "$(date)    Waiting for file update ... \r"
    sleep 1
 fi

done
