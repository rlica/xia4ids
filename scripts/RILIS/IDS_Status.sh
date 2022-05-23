#!/bin/bash
echo "DAQ status copying..." &
rm -f status.txt
OUTPUT=$(ssh pixie16@pixieids "tmux send-keys -t poll2:1.0 "status" Enter && sleep 1 && tmux capture-pane -pt poll2:1.0") 

STATUS=$(echo $OUTPUT | awk '{for(i=1;i<=NF;i++) if($i~/running/) {print $(i+2); exit;} }')

echo $OUTPUT
echo $STATUS

if [ $STATUS == "No" ]; 
	then
    	echo "STOPPED " > status.txt
    else
    	echo "RUNNING " > status.txt
fi


