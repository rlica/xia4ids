#!/usr/bin/env python3

#Simple XIA Pixie-16 DAQ GUI
#poll2 needs to be running in tmux
#
#R. Lica, May 2021
#Last change 24.06.2021

#Prerequisites:
#pip3 install pysimplegui
#yum install python3-tkinter

#Add the following lines in the .bashrc file:
##### Control poll2 from outside the tmux session (assuming session name 'poll2' and poll2 is started in pane 1.0)
### alias pixie_status="tmux send-keys -t poll2:1.0 \"status\" Enter && sleep 1 && tmux capture-pane -pt poll2:1.0"
### alias pixie_start="tmux send-keys -t poll2:1.0 \"run\" Enter && sleep 1 && tmux capture-pane -pt poll2:1.0"
### alias pixie_stop="tmux send-keys -t poll2:1.0 \"stop\" Enter && sleep 1 && tmux capture-pane -pt poll2:1.0"
### export CURRENT_EXP=/Data_Dir/2021/Tests_SiPIN_Apr2021
### the raw data should be placed in $CURRENT_EXP/RAW/

### The code will save automatically the current.set from ~/poll/ in $CURRENT_EXP/RAW/  


import sys
import os
import glob
import re
import subprocess
import time
from datetime import datetime
import PySimpleGUI as sg
from shutil import copyfile



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

def elog_read(window, values):
	with open(values['folder']+LOGFILE, 'r') as file_object:
		window['output'].update(file_object.read())

#Building the GUI window
sg.theme('Light Blue 2')
layout = [[sg.Text('Logfile entry:', font='Helvetica 18')],
          [sg.Multiline(size=(76,5), key='input', do_not_clear=False)],
          [sg.Button('RUN',  button_color='sea green', font='Helvetica 18', size=(4,2)), 
           sg.Button('STOP',  button_color='indian red', font='Helvetica 18', size=(4,2)),
           sg.Text(size=(14, 1), font=('Helvetica', 20), background_color='white', justification='center', key='status'),
           sg.Button('STAT', font='Helvetica 18', size=(4,2)), 
           sg.Exit(font='Helvetica 18', size=(4,2))],
          [sg.Multiline(size=(76,30), autoscroll=True, key='output')],
          [sg.Text('Exp. folder', size=(10, 1),font='Helvetica 14'), 
           sg.Input(default_text = FOLDER, key='folder'), 
           sg.FolderBrowse(initial_folder = FOLDER)],
         ]
window = sg.Window('IDS Simple DAQ Control', layout)


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
		copyfile('/home/pixie16/poll/current.set', FOLDER+'/Run%d.set' % run_number)
		
	if event == 'STOP':
		subprocess.run(['/bin/bash', '-i', '-c', 'pixie_stop'])
		time.sleep(1)
		window['status'].update("Stopped #%d" % get_run_number(), background_color='indian red')
		elog_new_entry(values, 'Stopped')
		elog_read(window, values)
		
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
