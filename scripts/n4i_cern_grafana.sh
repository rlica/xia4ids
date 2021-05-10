#!/bin/bash
#
# J. Cubiss
#
##############################

NUMBER_OF_BLOCKS=50           
WRITE_ROOT='n'

RAW_PATH='/home/TapeData/ISBON/'
CONFIG_FILE='/home/idsuser/Experiments/2017/ISBON/root/conf/config_rate'   
EXPERIMENT='ISBON'

##############################

OLD_FILE_SIZE=0              

while [ 1 = 1 ] ; do

 INPUT_FILE="$(ls -t $RAW_PATH* | head -1)"
 NEW_FILE_SIZE=$(stat -c%s "$INPUT_FILE")


 if [ $NEW_FILE_SIZE -ne $OLD_FILE_SIZE ];
  then
    n4i-fail $CONFIG_FILE $INPUT_FILE << echo
    $NUMBER_OF_BLOCKS     
    $WRITE_ROOT
echo

# Send rates to grafana
TOTAL_LINES=$(cat n4i_Run0.LOG | wc -l)

for ((LINE=3; LINE<=TOTAL_LINES; LINE++)); 

do

# Read values for the total no. of events, failed, pileup, rate and detector type
 TOTAL=$(awk -v i=$LINE 'NR==i { print $3 }' "n4i_Run0.LOG")
  FAIL=$(awk -v i=$LINE 'NR==i { print $4 }' "n4i_Run0.LOG")
PILEUP=$(awk -v i=$LINE 'NR==i { print $5 }' "n4i_Run0.LOG")
  RATE=$(awk -v i=$LINE 'NR==i { print $6 }' "n4i_Run0.LOG")
   DET=$(awk -v i=$LINE 'NR==i { print $7 }' "n4i_Run0.LOG")

# Set channeil number for detectors to read nice in grafana
if [ ${LINE} -ge 13 ];
  then
    CHAN=$(expr ${LINE} - 3)
  else
    CHAN=0$(expr ${LINE} - 3)
fi

# Send values red from file to database that grafana will read from
curl -i -XPOST 'https://ids-status.web.cern.ch/write?db=ids' --data-binary "${EXPERIMENT},type=TOTAL,chan=${CHAN},det=${DET} value=${TOTAL}"   >> /dev/null 
curl -i -XPOST 'https://ids-status.web.cern.ch/write?db=ids' --data-binary "${EXPERIMENT},type=FAIL,chan=${CHAN},det=${DET} value=${FAIL}"     >> /dev/null 
curl -i -XPOST 'https://ids-status.web.cern.ch/write?db=ids' --data-binary "${EXPERIMENT},type=PILEUP,chan=${CHAN},det=${DET} value=${PILEUP}" >> /dev/null 
curl -i -XPOST 'https://ids-status.web.cern.ch/write?db=ids' --data-binary "${EXPERIMENT},type=RATE,chan=${CHAN},det=${DET} value=${RATE}"     >> /dev/null 
	
done

# remove log file
rm -f n4i_Run0.LOG

    printf "$(date)    $(du -h $INPUT_FILE) \n"
    sleep 3
    OLD_FILE_SIZE=$NEW_FILE_SIZE
    	
   else
    printf "$(date)    Waiting for file update ... \r"
    sleep 1
 fi

done
