#!/usr/bin/env python3

#Simple XIA Pixie-16 DAQ GUI
#poll2 needs to be running in tmux
#
#R. Lica (2021), C. Page (2022)
#Last change 09.05.2022

#Prerequisites:
#pip3 install pysimplegui
#yum install python3-tkinter

#Add the following lines in the .bashrc file:
##### Control poll2 from outside the tmux session (assuming session name 'poll2' and poll2 is started in pane 1.0)
### alias pixie_status="tmux send-keys -t poll2:1.0 \"status\" Enter && sleep 1 && tmux capture-pane -pt poll2:1.0"
### alias pixie_start="tmux send-keys -t poll2:1.0 \"run\" Enter && sleep 1 && tmux capture-pane -pt poll2:1.0"
### alias pixie_stop="tmux send-keys -t poll2:1.0 \"stop\" Enter && sleep 1 && tmux capture-pane -pt poll2:1.0"
### export CURRENT_EXP=/Data_Dir/2021/the_current_exp_folder

### The raw data should be placed in $CURRENT_EXP/RAW/
### There is a function in .bashr to create new exp folders: 'new_exp_folder'

### The code will perform the following operations:
### - start/stop the Pixie DAQ
### - save the run information in the $CURRENT_EXP/elog.txt file together with other comments inserted by the user
### - send the comments also to the InlufxDB database to be displayed in Grafana
### - save automatically the current.set from ~/poll/ in $CURRENT_EXP/SET/  


import sys
import os
import glob
import re
import subprocess
import time
from datetime import datetime
import PySimpleGUI as sg
from shutil import copyfile
import requests

### Needed for Grafana
DATABASE='https://dbod-ids-db.cern.ch:8080/write?db=ids'
LOGIN='admin'
PASSWORD='hello_is_it_rates_youre_looking_for'
TABLE_NAME='pixie'

from requests.packages.urllib3.exceptions import InsecureRequestWarning
requests.packages.urllib3.disable_warnings(InsecureRequestWarning)
### 

LOGFILE = "/elog.txt" #default name for the logfile 
FOLDER = os.getenv('CURRENT_EXP')

def get_run_number():
	list_of_files = glob.glob(FOLDER+'/RAW/*.ldf') #Note: always update the $CURRENT_EXP env variable in .bashrc
	latest_file = max(list_of_files, key=os.path.getctime)
	return int(re.search('run_(\d+)', latest_file).group(1))

def elog_new_entry(values, string):
	with open(values['folder']+LOGFILE, 'a+') as file_object:
		file_object.writelines('\n'+datetime.now().strftime("%d/%m/%Y %H:%M:%S"))
		file_object.writelines('\t ' + string + ' RUN #%d' % get_run_number())
		file_object.writelines('\n'+values['input'])

def elog_comment(values):
	with open(values['folder']+LOGFILE, 'a+') as file_object:
		file_object.writelines('\n'+datetime.now().strftime("%d/%m/%Y %H:%M:%S"))
		file_object.writelines('\n'+values['input'])


def elog_read(window, values):
	with open(values['folder']+LOGFILE, 'r') as file_object:
		window['output'].update(file_object.read())
		
def grafana_new_entry(values, run_number, status):
	# Sending elog entry string to InfluxDB
	fullString = TABLE_NAME + ' ELOG_RUN=' + ('%d' % run_number) + ',ELOG="' + ('%s' % status+values['input']) + '"\n'  
	r = requests.post(DATABASE, auth=(LOGIN, PASSWORD), data=fullString, verify=False, timeout=10)
	print(r)
	

#Building the GUI window
sg.theme('Light Blue 2')
layout = [[sg.Text('Logfile entry:', font='Helvetica 18')],
          [sg.Multiline(size=(100,8), key='input', do_not_clear=False)],
          [sg.Button('RUN',  button_color='sea green', font='Helvetica 18', size=(7,2)), 
           sg.Button('STOP',  button_color='indian red', font='Helvetica 18', size=(7,2)),
           sg.Text(size=(14, 1), font=('Helvetica', 20), background_color='white', justification='center', key='status'),
		   sg.Button('Comment',  button_color='gold', font='Helvetica 18', size=(7,2)), 
           sg.Button('STAT', font='Helvetica 18', size=(4,2)), 
           sg.Exit(font='Helvetica 18', size=(4,2))],
          [sg.Multiline(size=(100,30), autoscroll=True, key='output')],
          [sg.Text('Exp. folder', size=(10, 1),font='Helvetica 14'), 
           sg.Input(default_text = FOLDER, key='folder'), 
           sg.FolderBrowse(initial_folder = FOLDER)],
         ]
window = sg.Window('IDS Simple DAQ Control', layout, location=(0,0)) #location=(0,0) /JC


# Event Loop
while True:  
	event, values = window.read()
		
	if event == sg.WIN_CLOSED or event == 'Exit':
		break

	if event == 'RUN':
		subprocess.run(['/bin/bash', '-i', '-c', 'pixie_start'])
		time.sleep(1)
		run_number = get_run_number()
		window['status'].update("Running #%d" % run_number, background_color='sea green')
		elog_new_entry(values, 'Started')
		elog_read(window, values)
		copyfile('/home/pixie16/poll/current.set', FOLDER+'/SET/Run%d.set' % run_number)
		grafana_new_entry(values, run_number, 'Started. ')
	
	if event =='Comment':
		elog_comment(values)
		elog_read(window, values)

	if event == 'STOP':
		subprocess.run(['/bin/bash', '-i', '-c', 'pixie_stop'])
		time.sleep(1)
		run_number = get_run_number()
		window['status'].update("Stopped #%d" % get_run_number(), background_color='indian red')
		elog_new_entry(values, 'Stopped')
		elog_read(window, values)
		grafana_new_entry(values, run_number, 'Stopped. ')
		
	if event == 'STAT':
		status_message_list = subprocess.check_output(['/bin/bash', '-i', '-c', 'pixie_status']).decode("utf-8").split()
		acq_running = status_message_list[status_message_list.index('running')+2]
		if acq_running == 'Yes':
			window['status'].update("Running #%d" % get_run_number(), background_color='sea green')
		elif acq_running == 'No':
			window['status'].update("Stopped #%d" % get_run_number(), background_color='indian red')
		else:
			window['status'].update("No status #%d" % get_run_number())
			 
		elog_read(window, values)
			
window.close()

