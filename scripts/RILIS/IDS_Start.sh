#!/bin/bash
rm -f out.txt &
#rm -f out.txt spec.a8 &
#ssh pixie16@pixieids "rm -f /home/pixie16/RILIS/temp.rates" &
echo "Starting DAQ" &

#START ONLY PIXIE
ssh pixie16@pixieids "tmux send-keys -t poll2:1.0 "run" Enter && sleep 1 && tmux capture-pane -pt poll2:1.0" &

