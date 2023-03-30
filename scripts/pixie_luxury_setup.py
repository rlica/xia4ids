#!/usr/bin/env python3

#User setup
#Luxury Setup for Pixie 16
#poll2 needs to be running in tmux
#
#C. Page 2022
#Last change 11/10/2022

#C Page created and made first version
#This programme allows you to write parameters to pixie for a new setup quickly
#Prerequisites:
#pip3 install pysimplegui
#yum install python3-tkinter

#Direct interaction with the pixie requires tmux session to be initialised in the normal way

#Add the following lines in the .bashrc file:
##### Control poll2 from outside the tmux session (assuming session name 'poll2' and poll2 is started in pane 1.0)


#TO ADD (please add any suggestions here :) ) 
#   Checking module parameters are compatible different presets
# 
#
#
#
#


from tkinter import SEPARATOR
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
no_chn_params = len(CHANNEL_PARAMS)
no_mod_params = len(MODULE_PARAMS)

BUTTON_FONT = 'Helvetica 8'
TITLE_FONT = 'Helvetica 12'
BUTTON_SIZE = (18,2)
HALF_BUTTON_SIZE = (9,2)

SEP = "________________________________________________________________"
SEPARATE = "\n________________________________________________________________\n"

NO_MODULES = int(input("please enter the number of modules you are using: "))
NO_CHANNELS = 16


window = None
class Channel():

    def __init__(self,module,channel,preset):
        self.module = module
        self.channel = channel
        self.preset = preset
        self.changed = False
        self.sfr = -np.inf #slow filter range - module parameter
        

    def __str__(self):
        return f"{self.module}.{self.channel}"

    def get_param(param_name):
        return self.preset.parameters[param_name]

class Module():

    def __init__(self,module,dt):
        self.module = module
        self.parameters = {}
        self.param_changed = {}
        self.dt =dt
        for param in MODULE_PARAMS:
            self.parameters[param] = -np.inf
            self.param_changed[param] = False

class Preset():

    def __init__(self,name):
        self.name = name
        self.parameters = {}
    def __str__(self):
        return self.name

        
def read_presets():
    file_path = "/home/pixie16/programs/CP/presets.csv"
    input_file = open(file_path,"r")
    lines = input_file.readlines()
    no_presets = len(lines[0].split(","))-1
    presets = []
    for i in range(no_presets):
        presets.append(Preset(lines[0].split(",")[i+1]))
        

    for line_no in range(len(lines)):
        if line_no>0:
            line_vals = lines[line_no].split(",")
            
            #print(line_vals)
            #print(f"{line_vals[0]} is the value")
            if line_vals[0] in CHANNEL_PARAMS :
                line_vals[-1]=line_vals[-1][:-1]
                for preset_no in range(1,no_presets-1):
                    value_index = preset_no-1#Index:    0   , 1   2   3
                    #                        #Value:    name, v1, v2, v3
                    #                        # - 1 therefore needed
                    
                    try:
                        val =float(line_vals[preset_no])
                        presets[value_index].parameters[line_vals[0]] = val
                    except ValueError:
                        print(f"{preset_no} {line_no} {line_vals[preset_no]} is exceptional" )
                        
            elif line_vals[0] in MODULE_PARAMS:
                line_vals[-1]=line_vals[-1][:-1]

                for preset_no in range(1,no_presets-1):
                    value_index = preset_no-1#Index:    0   , 1   2   3
                    try:
                        val =float(line_vals[preset_no])
                        presets[value_index].parameters[line_vals[0]] = val
                    except ValueError:
                        print(f"{preset_no} {line_no} {line_vals[preset_no]} is exceptional" )

            else:
                
                print(f"PARAMETER NOT FOUND {line_no} {line_vals[0]}")
    return presets

def get_run_number():
        FOLDER = os.getenv('CURRENT_EXP')
        list_of_files = glob.glob(FOLDER+'/RAW/*.ldf') #Note: always update the $CURRENT_EXP env variable in .bashrc
        latest_file = max(list_of_files, key=os.path.getctime)
        return int(re.search('run_(\d+)', latest_file).group(1))

def get_exp_number():
    return os.getenv('CURRENT_EXP').split("/")[-1]

def get_sfr0(channels,m):
    for c in range(16):
        button_key = get_chan_key(m,c)
        if channels[button_key].changed:
            return channels[button_key].sfr



def check_sfr(channels,m,c):
    #print(f"m is {m}")
   # print(channels.keys())
    sfr0 = get_sfr0(channels,m)
    same = []
    for channel_no in range(NO_CHANNELS):
        button_key = get_chan_key(m,channel_no)
        #print(button_key)
        chan = channels[button_key]
        if chan.sfr !=sfr0 and chan.changed:
            same.append(False)
        else:
            same.append(True)
    
    return same


def update_pixie(channels, parameters):
    # try:
    start_time = str(datetime.now()).split(".")[0].replace(" ","_").replace(":","-")
    sucessful = 0
    failed = 0 
    
    s_log_text = "\n\nLog of sucesful writting:\n"
    f_log_text = "\n\nLog of failed writting:\n"
    
    for chan_key in channels:
        chan = channels[chan_key]
        
        for param in parameters:
            try:
                 if(chan.changed):
                    try:
                        value = chan.preset.parameters[param]
                    except KeyError:
                        print(f"failed with {param} due to key error")
                        print(chan.preset.parameters.keys())
                        value = 1
                    module = chan.module
                    channel = chan.channel
                    if chan.preset.name=="OFF" and param!="CHANNEL_CSRA":
                        pass
                    else:
                        poll_cmd = f'tmux send-keys -t poll2:1.0 \"pwrite {module} {channel} {param} {value}\" Enter && tmux capture-pane -pt poll2:1.0'
                        print(poll_cmd)
                        # result = subprocess.run(['/bin/bash', '-i', '-c', poll_cmd], stdout=subprocess.PIPE).stdout.decode("utf-8")
                        # lines = result.split("\n")
                        
                        
                        
                        # if lines[-4][-3:-1]=="OK" or lines[-3][-3:-1]=="OK" or lines[-5][-3:-1]=="OK":
                        #     sucessful+=1
                        #     s_log_text+=f"\n success number {sucessful}\n{result[-6:-3]}\n{SEPARATE}n"
                        # else:
                        #     print("failed on\n")
                        #     print(lines[-5:] )
                        #     failed+=1
                        #     f_log_text+=f"\n failure number {failed}: {poll_cmd}\n{result[-6:-3]}\n{SEPARATE}n"

            except IndexError:
                failed +=1 
                s_log_text+=f"\nPYTHON INDEX ERROR!!! failure number {failed}\n{result[-6:-3]}\n{SEPARATE}\n"
    
        ###############################################################
        #########need to implement module parameter writing properly
        ###############################################################

    # for param in MODULE_PARAMS:
    #     for module in range(no_modules):
    #         try:
    #             if modules[module].param_changed[param]:
    #                 value = modules[module].parameters[param]
    #                 poll_cmd = f'tmux send-keys -t poll2:1.0 \"pmwrite {module} {param} {int(value)}\" Enter && tmux capture-pane -pt poll2:1.0'
    #                 result = subprocess.run(['/bin/bash', '-i', '-c', poll_cmd], stdout=subprocess.PIPE).stdout.decode("utf-8")
    #                 lines = result.split("\n")
    #                 if lines[-4][-3:-1]=="OK" or lines[-3][-3:-1]=="OK" or lines[-5][-3:-1]=="OK":
    #                     sucessful+=1
    #                     s_log_text+=f"\n success number {sucessful}\n{result[-6:-3]}\n{SEPARATE}n"
    #                 else:
    #                     failed+=1
    #                     f_log_text+=f"\n failure number {failed}\n{result[-6:-3]}\n"
    #                     f_log_text+=f"\n failure number {failed}: {poll_cmd}\n{result[-6:-3]}\n{SEPARATE}n"
# 
            # except IndexError:
                # failed +=1 
                # s_log_text+=f"\nPYTHON INDEX ERROR!!! failure number {failed}\n{result[-6:-3]}\n{SEPARATE}n"
        
    # except:
    #     print("something failed")
    # else:
    #     print("nothing failed")
    end_time = str(datetime.now()).split(".")[0].replace(" ","_").replace(":","-")          
    exp_no = get_exp_number()
    run_no = get_run_number()

    folder_path = f"/home/pixie16/poll/parameters/{exp_no}/auto_write_logs"
    if not os.path.isdir(folder_path):
                print("parameter folder for current experiment not found. creating one now...")
                os.mkdir(folder_path)
    
    log_file = open(f"{folder_path}/auto_write_log_{start_time}_run_{run_no}","w")
    
    log_file.write(f"Auto_write started at {start_time} and completed at {end_time}. Current run is #{run_no} of experiment {exp_no}\n")
    log_file.write(f"Ended with {sucessful} succesfully written parameters and {failed} unsuccesfully written parameters")
    log_file.write(SEPARATE)
    log_file.write(f_log_text)
    log_file.write(s_log_text)
    log_file.close()
    poll_cmd = f'tmux send-keys -t poll2:1.0 \"adjust_offsets -1\" Enter && tmux capture-pane -pt poll2:1.0'
    # subprocess.run(['/bin/bash', '-i', '-c', poll_cmd], stdout=subprocess.PIPE).stdout.decode("utf-8")
    print(poll_cmd)

def get_mod_channel(button_name):
    vals = button_name.split("C")
    return int(vals[0][1:]), int(vals[1])


def get_chan_key(module,channel):
    return f"M{module}C{channel}"

def main():
    sg.theme('Light Blue 2')

    presets = read_presets()
    no_presets = len(presets)
    presets_dict = {}
    for pre in presets:
        presets_dict[str(pre)] = pre

    param_names = presets[0].parameters.keys()
    
    preset_names = []
    preset_list_size =(no_presets//10 +1 , 10)
    preset_buttons = [[None] * 10 for i in range(preset_list_size[0])]

    
    
    for button_no,preset in enumerate(presets):
        i = button_no//10
        j = button_no%10
        preset_names.append(str(preset))
        preset_buttons[i][j] = sg.Button(str(preset),button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE,key=str(preset))
        
    preset_buttons[-1] = list(filter(lambda item: item is not None, preset_buttons[-1]))

    preset_names.append("clear")
    preset_buttons[-1].append(sg.Button("clear",button_color='red', font=BUTTON_FONT, size=BUTTON_SIZE,key="clear"))

    channels = {}
    channel_keys = []
    channel_buttons =[]
    module_keys =[]
    col_labels = [sg.Text(" ", size=HALF_BUTTON_SIZE,font=TITLE_FONT)]
    for module_no in range(NO_MODULES):
        module_keys.append(f"mod-{module_no}")
        col_labels.append(sg.Button(module_no+1, size=BUTTON_SIZE,font=BUTTON_FONT,key=f"mod-{module_no}"))
    
    for channel_no in range(NO_CHANNELS):
        channel_row = [sg.Text(channel_no, size=HALF_BUTTON_SIZE,font=TITLE_FONT)]
        for module_no in range(NO_MODULES):
            try:
                button_key = get_chan_key(module_no,channel_no)#f"M{module_no}C{channel_no}"
                channels[button_key] = Channel(module_no,channel_no,presets[0])
                channel_row.append(sg.Button("None",button_color='blue', font=BUTTON_FONT, size=BUTTON_SIZE,key=button_key))
                channel_keys.append(button_key)
            except IndexError:
                print(f"{module_no} {channel_no}")
        channel_buttons.append(channel_row)
    
    
    layout = [
        [
            sg.Button("Send",button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE,key="send"),
            sg.Button("Exit",button_color='red', font=BUTTON_FONT, size=BUTTON_SIZE,key="exit")
        ],
        preset_buttons,
        col_labels,
        channel_buttons
    ]
    current_preset = preset_names[0]
    window = sg.Window('IDS Luxury Setup', layout,location=(0,0))
    while True:
        try:
            event, values = window.read()
            if event == sg.WIN_CLOSED or event == 'exit':
                break


            if event in channel_keys:
                m,c = get_mod_channel(event)
                if current_preset != "clear":
                    channels[event].preset = presets_dict[current_preset]   
                    channels[event].changed = True
                    channels[event].sfr = presets_dict[current_preset].parameters["SLOW_FILTER_RANGE"]
                    channel_buttons[c][m+1].update(current_preset)
                    #print(channels[event].sfr)
                    sfr_same = check_sfr(channels,m,c)
                    print(sfr_same)
                    for i,same in enumerate(sfr_same):
                        colour = "red"
                        if same:
                            colour = "blue"

                        channel_buttons[i][m+1].update(button_color=colour)
    
                else:
                    channels[event].changed = False
                    channel_buttons[c][m+1].update("None")

            
            if event in module_keys:
                m = int(event[4:])
                print(f"m is {m}")
                if current_preset != "clear":
                    for c in range(NO_CHANNELS):
                        ch_key = get_chan_key(m,c)
                        channels[ch_key].preset = presets_dict[current_preset]   
                        print(f"c is {c} channels is len {len(channel_buttons)}. channel buttons is len {len(channel_buttons[c])}")
                        channel_buttons[c][m+1].update(current_preset,button_color="blue")
                        channels[ch_key].changed = True
                else:
                    for c in range(NO_CHANNELS):
                        ch_key = get_chan_key(m,c)
                        channels[ch_key].changed = False 
                        channel_buttons[c][m+1].update("None")




            if event in preset_names:
                print(event)
                current_preset = event
            
            if event =="send":
                
                update_pixie(channels,param_names)

        except KeyboardInterrupt:
            break

main()


