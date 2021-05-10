#!/usr/bin/env python3

#Grafana Monitoring for XIA Pixie-16 DAQ
#poll2 monitor script needs to be running 
#Data taken from /home/pixie16/poll/monitor.txt using
#	monitor | tee monitor.txt
#
#R. Lica, May 2021

import requests


from requests.packages.urllib3.exceptions import InsecureRequestWarning

requests.packages.urllib3.disable_warnings(InsecureRequestWarning)


DATABASE='https://dbod-ids-db.cern.ch:8080/write?db=ids'
LOGIN='admin'
PASSWORD='hello_is_it_rates_youre_looking_for'
TABLE_NAME='pixie'

PATH = "/home/pixie16/poll"



file = open("{}/monitor.txt".format(PATH))
lines = file.readlines()
file.close()



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

# Check file integrity								
if (len(lines) < 18 or clean_line(0)[0] != "C00"):
	print("ERROR: Check {}/monitor.txt file".format(PATH))
	#print(lines)
	exit()
  

MODULES = int((len(clean_line(0))-1 )/4)
					
fullString = ""

# Adding all the channels with data into a single string to post 
for mod in range(MODULES):
	for chan in range(16):
		if TOTAL(mod, chan) > 0:
			fullString += dataString(mod, chan)

print(fullString) 

# Sending rates data to InfluxDB
r = requests.post(DATABASE, auth=(LOGIN, PASSWORD), data=fullString, verify=False, timeout=10)
print(r)


