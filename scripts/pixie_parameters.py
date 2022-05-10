#!/usr/bin/env python3

#Simple XIA Pixie-16 DAQ GUI
#poll2 needs to be running in tmux
#
#C. Page 2022
#Last change 09/05/2022

#Prerequisites:
#pip3 install pysimplegui
#yum install python3-tkinter

#Add the following lines in the .bashrc file:
##### Control poll2 from outside the tmux session (assuming session name 'poll2' and poll2 is started in pane 1.0)
### alias pixie_dump="tmux send-keys -t poll2:1.0 \"dump\" Enter && sleep 1 && tmux capture-pane -pt poll2:1.0"

### The raw data should be placed in $CURRENT_EXP/RAW/
### There is a function in .bashr to create new exp folders: 'new_exp_folder'

### The code will perform the following operations:
### Display all current module/channel parameters for the connected pixie modules
### To add: Write new parameters to a text file
### To add: Write new parameters to a Binary file that can be read by the pixie


import numpy as np
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
import warnings

#CHANNEL_PARAMS are the pixie parameters which can be changed for each individual channel
CHANNEL_PARAMS = ("TRIGGER_RISETIME",
"TRIGGER_FLATTOP",
"TRIGGER_THRESHOLD",
"ENERGY_RISETIME",
"ENERGY_FLATTOP",
"TAU",
"TRACE_LENGTH",
"TRACE_DELAY",
"VOFFSET",
"XDT",
"BASELINE_PERCENT",
"EMIN",
"BINFACTOR",
"CHANNEL_CSRA",
"CHANNEL_CSRB",
"BLCUT",
"ExternDelayLen",
"ExtTrigStretch",
"ChanTrigStretch",
"FtrigoutDelay",
"FASTTRIGBACKLEN",
"VetoStretch",
"CFDDelay",
"CFDScale",
"CFDThresh",
"QDCLen0",
"QDCLen1",
"QDCLen2",
"QDCLen3",
"QDCLen4",
"QDCLen5",
"QDCLen6",
"QDCLen7")

#MODULE_PARAMS are the module-wide parameters
MODULE_PARAMS = (
"MODULE_CSRA",
"MODULE_CSRB",
"MODULE_FORMAT",
"MAX_EVENTS",
"IN_SYNCH",
"SLOW_FILTER_RANGE",
"FAST_FILTER_RANGE",
"MODULE_NUMBER",
"TrigConfig0",
"TrigConfig1",
"TrigConfig2",
"TrigConfig3",
"SYNCH_WAIT")

#Set up some global variables
BUTTON_FONT = 'Helvetica 8'
TITLE_FONT = 'Helvetica 14'
BUTTON_SIZE = (18,2)

CHAN_SELECT = True
window = None


"""
Channel and Module class store the data read in from the config file. Each class instance of Channel represents a specific channel in a
specific module while each instance of Module represents a specific module. The Channel/Module insstance contains (channel and) module number
and a dictionary whose keys are the different parameter names in the lists above.

"""
class Channel():

    def __init__(self,module,channel):
        self.module = module
        self.channel = channel
        self.parameters = {}
        for param in CHANNEL_PARAMS:
            self.parameters[param] = -np.inf

    def __str__(self):
        return f"{self.module},{self.channel}"
class Module():

    def __init__(self,module):
        self.module = module
        self.parameters = {}
        for param in MODULE_PARAMS:
            self.parameters[param] = -np.inf



def check_no_channels(lines):
    """
    Short function to check the total number of channels across all modules.
    It is assumed that there are 16 channels per module later on using this number
    """
    channel =0
    while lines[channel].split("\t")[2] == 'TRIGGER_RISETIME':
        channel +=1
    return channel

def readFile(channels,modules,line):
    """
    function to read in the text config file. the variable line is a string which contains the text of the current line of the
    text file which is split by tabs.
    """
    line = line.replace("\n","").split("\t")
    if len(line) ==4:
        #line length 4 means channel parameter
        
        line[0],line[1],line[3] = int(line[0]),int(line[1]),float(line[3])
        if line[2] in channels[line[0]][line[1]].parameters:
            channels[(line[0])][(line[1])].parameters[line[2]] = line[3]
        else:
            print(f"unknown key - {line[2]}")
    elif len(line) ==3:
        #line length 3 means module parameter
        line[0],line[2] = int(line[0]),float(line[2])
        if line[1] in modules[line[0]].parameters:
            modules[line[0]].parameters[line[1]] = line[2]
        else:
            print(f"unknown key - {line[1]}")


def setup(file_path="./IS659_4May2022.txt"):
    """
    function to read in all data and and return it as a list of lists of instances of Channel and a list of instances of Module
    """
    input_file = open(file_path,"r")
    
    lines = input_file.readlines()
    

    noChannels = check_no_channels(lines)
    noModules = noChannels//16

    channels = []
    modules = []
    for moduleNo in range(noModules):
        currentModule = []
        for channelsNo in range(16):
            currentModule.append(Channel(moduleNo,channelsNo))
        channels.append(currentModule)
        modules.append(Module(moduleNo))


    for i in range(len(lines)):
        readFile(channels,modules,lines[i])

    return noChannels,noModules,channels,modules


def output_channel_param(channels,param,noChannels,noModules):
    if param in channels[0][0].parameters.keys():
        ret_str = f"{param}\n\n"
        for mod in range(noModules):
            ret_str+=f"\t\tModule {mod}"
        for chan in range(16):
            ret_str+=f"\n{chan}"
            for mod in range(noModules):
                ret_str+=f"\t\t{channels[mod][chan].parameters[param]}"
            
        return ret_str
    else:
        print(f"ERROR!!!!\nKEY NOT RECOGNISED\nYOU GAVE {param}")
        

def output_module_param(modules,param,noModules):
    if param in modules[0].parameters.keys():
        ret_str = f"{param}\n\n"
        for mod in range(noModules):
            ret_str+=f"\t\tModule {mod+1}"
        ret_str+="\n"        
        for mod in range(noModules):
            ret_str+=f"\t\t{modules[mod].parameters[param]}"
            
        return ret_str

    else:
        print(f"ERROR!!!!\nKEY NOT RECOGNISED\nYOU GAVE {param}")

def write_txt(text,channels,modules):
    pass





def main():
    
    file_loaded = False
    noChannels=-1
    noModules=-1
    channels=[]
    modules=[]


    sg.theme('Light Blue 2')
    button_line1 = []
    button_line2 =[]
    button_line3 =[]
    button_line4=[]
    
    button_line5=[]
    button_line6=[]
    
    for i in range(11):
        try:
            button_line1.append(sg.Button(CHANNEL_PARAMS[i],button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE))
            button_line2.append(sg.Button(CHANNEL_PARAMS[i+11],button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE))
            button_line3.append(sg.Button(CHANNEL_PARAMS[i+22],button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE))
            button_line4.append(sg.Button(CHANNEL_PARAMS[i+33],button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE))
        except IndexError:
            pass
        try:
            button_line5.append(sg.Button(MODULE_PARAMS[i],button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE))
            button_line6.append(sg.Button(MODULE_PARAMS[i+11],button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE))
        except IndexError:
            pass
    layout = [
            [sg.Text('config file path', size=(15, 1),font=TITLE_FONT), 
            sg.Input(default_text = "C:", key='file'), 
            sg.FileBrowse(initial_folder = "C:"),
            sg.Button("Load",button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE),
            sg.Exit(font=BUTTON_FONT, size=(4,2))],
            
            [sg.Text('Individual Channel parameters:', size=(24, 1),font=TITLE_FONT)],
            button_line1,
            button_line2,
            button_line3,
            button_line4,
            [sg.Text("Full module parameters:", size=(20, 1),font=TITLE_FONT)],
            button_line5,
            button_line6,
            [sg.Multiline(size=(225,20), autoscroll=True, key='output',font='Helvetica 8')], 
            [sg.Input(default_text = "Path to dump", key='dump_path'),
            sg.FolderBrowse(initial_folder = " /home/pixie16/poll/"),
            sg.Button("Dump",button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE),
            sg.Button("Dump and load",button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE),
           
            ]
            ]
    window = sg.Window('IDS Pixie parameters', layout)
    while True:  
        event, values = window.read()

        if event =='Load':
            try:
                noChannels,noModules,channels,modules = setup(values['file'])
            except FileNotFoundError:
                window['output'].update("Error, the file could not be found. Please check path")
            except KeyError:
                window['output'].update("Warning - there was an issue with the parameter names. Please check terminal")
            else:
                window['output'].update("File loaded successfully")

            file_loaded = True
        if file_loaded:
            if event == sg.WIN_CLOSED or event == 'Exit':
                break

            if event in CHANNEL_PARAMS:
                string_to_write = output_channel_param(channels,event,noChannels,noModules)
                window['output'].update(string_to_write)
            
            if event in MODULE_PARAMS:
                string_to_write = output_module_param(modules,event,noModules)
                window['output'].update(string_to_write)
            
            if event =="Dump and load":
                subprocess.run(['/bin/bash', '-i', '-c', 'pixie_dump'])
                time.sleep(1)
                noChannels,noModules,channels,modules = setup(file_path=f"{values['dump_path']}/data.txt")
                try:
                    noChannels,noModules,channels,modules = setup(values['file'])
                except FileNotFoundError:
                    window['output'].update("Error, the file could not be found. Please check path")
                except KeyError:
                    window['output'].update("Warning - there was an issue with the parameter names. Please check terminal")
                else:
                    window['output'].update(f"current parameters dumped to {values['dump_path']}.File loaded successfully")
                
            if event == "write txt":
                write_txt(values,channels,modules)
        else:
            if event == sg.WIN_CLOSED or event == 'Exit':
                break

            if event in CHANNEL_PARAMS or event in MODULE_PARAMS or event =="dump" or event == "write txt":
                window['output'].update("\n\n\t\tNO FILE LOADED. PLEASE SELECT A FILE ABOVE!!!")
                

			
    window.close()
    
    

main()

