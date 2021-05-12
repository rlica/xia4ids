#!/usr/bin/env python3

#Grafana Monitoring for XIA Pixie-16 DAQ
#poll2 monitor script needs to be running 
#Data taken from /home/pixie16/poll/monitor.txt using
#	monitor | tee monitor.txt
#
#R. Lica, May 2021
#Last change 12.05.2021

DATABASE='https://dbod-ids-db.cern.ch:8080/write?db=ids'
LOGIN='admin'
PASSWORD='hello_is_it_rates_youre_looking_for'
TABLE_NAME='pixie'
FILENAME = "/home/pixie16/poll/monitor.txt"


import requests
import os
import time
from datetime import datetime
import signal

from requests.packages.urllib3.exceptions import InsecureRequestWarning
requests.packages.urllib3.disable_warnings(InsecureRequestWarning)


#Handling CTRL+C, sending to InfluxDB SCRIPT=0 
def handler(signum, frame):
    print('Exiting ...')
    requests.post(DATABASE, auth=(LOGIN, PASSWORD), data='pixie SCRIPT=0', verify=False, timeout=10)
    exit()

#Reading only the last entries in the file
def clean_line(chan):
	return lines[chan-18].replace("k", "E3").replace("|", " ").split()

def ICR(mod, chan):
	return float(clean_line(chan)[1+mod*4])

def OCR(mod, chan):
	return float(clean_line(chan)[2+mod*4])

def DATA(mod, chan):
	return float(clean_line(chan)[3+mod*4])	

def TOTAL(mod, chan):
	return float(clean_line(chan)[4+mod*4])	

# Format the data string to be sent to InfluxDB
def dataString(mod, chan):
	return	TABLE_NAME +	',Module='	+ ('%d' % mod) + \
							',Channel='	+ ('%d' % chan) + \
							' ICR='		+ ('%.1f' % ICR(mod, chan)) + \
							',OCR='		+ ('%.1f' % OCR(mod, chan)) + \
							',DATA='	+ ('%.1f' % DATA(mod, chan)) + \
							',TOTAL='	+ ('%.1f' % TOTAL(mod, chan)) + '\n'







#Start of the script:
requests.post(DATABASE, auth=(LOGIN, PASSWORD), data='pixie SCRIPT=0', verify=False, timeout=10)

#Check if file exists
if not os.path.exists(FILENAME):
	print("ERROR: File {} does not exist.".format(FILENAME))
	exit()


signal.signal(signal.SIGINT, handler)
									
#Reading the file size 
old_size = 0
new_size = os.stat(FILENAME).st_size 

while True:
		
	if new_size != old_size:
	
		file = open(FILENAME)
		lines = file.readlines()
		file.close()
		
		# Check monitor.txt file integrity								
		if (len(lines) < 18 or clean_line(0)[0] != "C00"):
			print("ERROR: Check {} file".format(FILENAME))
			requests.post(DATABASE, auth=(LOGIN, PASSWORD), data='pixie SCRIPT=0', verify=False, timeout=10)
			#print(lines)
			time.sleep(5)
			continue
			
		requests.post(DATABASE, auth=(LOGIN, PASSWORD), data='pixie SCRIPT=1', verify=False, timeout=10)
		
		#Get total number of modules from the number of columns written
		MODULES = int((len(clean_line(0))-1 )/4)
		
		#Get datarate
		DATARATE = float(clean_line(16)[5])
		if clean_line(16)[6] == 'MB/s':  #usually it is kB/s
			DATARATE = DATARATE*1000
		
		#Get total RUNTIME in seconds
		pt = datetime.strptime(clean_line(16)[2],'%H:%M:%S.%f')
		RUNTIME = pt.second + pt.minute*60 + pt.hour*3600
		
		#This is the full string of data to be sent to InfluxDB							
		fullString = TABLE_NAME + ' RUNTIME=' + ('%s' % RUNTIME) + ',DATARATE='	+ ('%.1f' % DATARATE) + '\n'  
		
		# Adding all the channels with data into a single string to post 
		for mod in range(MODULES):
			for chan in range(16):
				if TOTAL(mod, chan) > 0:
					fullString += dataString(mod, chan)
		
		print(fullString) 
				
		# Sending rates data to InfluxDB
		r = requests.post(DATABASE, auth=(LOGIN, PASSWORD), data=fullString, verify=False, timeout=10)
		print(r)
		
		#Updating the file size
		old_size = new_size
		new_size = os.stat(FILENAME).st_size
		time.sleep(3)
	
	else:
		requests.post(DATABASE, auth=(LOGIN, PASSWORD), data='pixie SCRIPT=2', verify=False, timeout=10)
		new_size = os.stat(FILENAME).st_size
		time.sleep(3)
		


