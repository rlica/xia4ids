#!/bin/bash
total1=0;
total2=0;
total3=0;
total4=0;
total5=0;
#count1=0;
#count2=0;
#count3=0;
#count4=0;
#count5=0;

RAW_PATH='/home/TapeData/I608_2/'
cd $RAW_PATH
RUN_NO="$(ls -1t | head -1 | sed 's/Run_//' | sed 's/_0//')"

cd /home/idsuser/RILIS/
#ave1=$(awk '{ total1 += $1; count1++ } END { print total1/count1 }' "temp_rates.dat")
#ave2=$(awk '{ total2 += $2; count2++ } END { print total2/count2 }' "temp_rates.dat")
#ave3=$(awk '{ total3 += $3; count3++ } END { print total3/count3 }' "temp_rates.dat")
#ave4=$(awk '{ total4 += $4; count4++ } END { print total4/count4 }' "temp_rates.dat")
#ave5=$(awk '{ total5 += $5; count5++ } END { print total5/count5 }' "temp_rates.dat")

ave1=$(awk '{ total1 += $1 } END { print total1 }' "temp_rates.dat")
ave2=$(awk '{ total2 += $2 } END { print total2 }' "temp_rates.dat")
ave3=$(awk '{ total3 += $3 } END { print total3 }' "temp_rates.dat")
ave4=$(awk '{ total4 += $4 } END { print total4 }' "temp_rates.dat")
ave5=$(awk '{ total5 += $5 } END { print total5 }' "temp_rates.dat")

echo "$RUN_NO	200	$ave1	$ave2	$ave3	$ave4	$ave5" > out.txt
rm -f temp_rates.dat
