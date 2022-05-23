#!/bin/bash
echo "DAQ Stopped...sorting" &
rm -f out.txt &
#rm -f out.txt spec.a8 &

#STOP Pixie
ssh pixie16@pixieids 'tmux send-keys -t poll2:1.0 "stop" Enter && sleep 1 && tmux capture-pane -pt poll2:1.0' &

sleep 1

#ssh pixie16@pixieids "/home/pixie16/RILIS/scan.sh"
#ssh pixie16@pixieids '/home/pixie16/RILIS/sort.sh'

ssh pixie16@pixieids '/home/pixie16/RILIS/sort_fast.sh'

echo "Copying outputfile" &
scp pixie16@pixieids:~/RILIS/out.txt . &
scp pixie16@pixieids:~/RILIS/spec.txt . &
