#!/usr/bin/env python3

#Simple XIA Pixie-16 DAQ GUI
#poll2 needs to be running in tmux
#
#R. Lica, May 2021
#Last change 22.05.2021

#Prerequisites:
#pip3 install pysimplegui
#yum install python3-tkinter

#Add the following lines in the .bashrc file:
##### Control poll2 from outside the tmux session (assuming session name 'poll2' and poll2 is started in pane 1.0)
### alias pixie_status="tmux send-keys -t poll2:1.0 \"status\" Enter && sleep 1 && tmux capture-pane -pt poll2:1.0"
### alias pixie_start="tmux send-keys -t poll2:1.0 \"run\" Enter && sleep 1 && tmux capture-pane -pt poll2:1.0"
### alias pixie_stop="tmux send-keys -t poll2:1.0 \"stop\" Enter && sleep 1 && tmux capture-pane -pt poll2:1.0"
### export CURRENT_EXP=/Data_Dir/2021/Tests_SiPIN_Apr2021



import sys
import os
import glob
import re
import subprocess
import time
from datetime import datetime
import PySimpleGUI as sg



LOGFILE = "/elog.txt" #default name for the logfile 
FOLDER = os.getenv('CURRENT_EXP')

def get_run_number():
	list_of_files = glob.glob(FOLDER+'/raw/*.ldf') #Note: always update the $CURRENT_EXP env variable in .bashrc
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
          [sg.Multiline(size=(70,5), key='input', do_not_clear=False)],
          [sg.Button('RUN',  font='Helvetica 18', size=(4,2)), 
           sg.Button('STOP', font='Helvetica 18', size=(4,2)), 
           sg.Exit(font='Helvetica 18', size=(4,2)),
		   sg.Text(size=(15, 1), font=('Helvetica', 20), background_color='white', justification='center', key='status')],
          [sg.Multiline(size=(70,30), autoscroll=True, key='output')],
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
		window['status'].update("Running #%d" % get_run_number(), background_color='green')
		elog_new_entry(values, 'Started')
		elog_read(window, values)
		
	if event == 'STOP':
		subprocess.run(['/bin/bash', '-i', '-c', 'pixie_stop'])
		time.sleep(1)
		window['status'].update("Stopped #%d" % get_run_number(), background_color='red')
		elog_new_entry(values, 'Stopped')
		elog_read(window, values)
		
window.close()
