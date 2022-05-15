#!/usr/bin/env python3

#Simple XIA Pixie-16 DAQ GUI
#poll2 needs to be running in tmux
#
#C. Page 2022
#Last change 15/05/2022

#C Page - added CSRA test with colour coding
#       - added compare button to compare two txts
#       - added but needing testing when not on experiment:
            # *dump button - this should make the pixie dump parameters to file with exp no and date and time
            # *parameters can be edited in the text window and then sent to the pixie
#Prerequisites:
#pip3 install pysimplegui
#yum install python3-tkinter

#Direct interaction with the pixie requires tmux session to be initialised in the normal way

#Add the following lines in the .bashrc file:
##### Control poll2 from outside the tmux session (assuming session name 'poll2' and poll2 is started in pane 1.0)


#To add:
#Merging load an browse buttons....perhaps. Given the compare button, this may not be needed/wanted.
#Colour coding buttons when they are pressed

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

CSRA_TRUE = [
"-",#                                   0
"-",#                                   1
"Good ChannelL",#                       2
"-",#                                   3
"-",#                                   4
"Trigger positive",#                    5
"-",#                                   6
"-",#                                   7
"Trace capture ENABLED ",#              8
"QDC Sums Enabled",#                    9
"Trigger mode CFD ENABLED",#            10
"Global trigger validation ENABLED",#   11
"Raw Energy sums Capture ENABLED",#     12
"Channel Trigger Validation ENABLED",#  13
"LOW gain",#                            14 #############CHECK WHICH WAY AROUND THIS IS!!!!!!!!!!!!!!!!!!!!!
"Pileup rejection ENABLED", #           15
"Inverse Pileup ENABLED",#              16
"-",#                                   17
"SHE Trace Capture ENABLED",#           18
"-",#                                   19
"-",#                                   20
"External Timestamping ENABLED",#       21
"External Fast Trigger ENABLED",#       22
"Override Peak Sample Point ENABLED"#   23
]


CSRA_FALSE =["-",#                      0
"-",#                                   1
"Bad Channel",#                         2
"-",#                                   3
"-",#                                   4
"Trigger Negative",#                    5
"-",#                                   6
"-",#                                   7
"Trace capture DISABLED",#              8
"QDC Sums DISABLED",#                   9
"Trigger mode CFD DISABLED",#           10
"Global trigger validation DISABLED",#  11
"Raw Energy sums capture DISABLED",#    12
"Channel Trigger Validation DISABLED",# 13
"HIGH gain",#                           14 #############CHECK WHICH WAY AROUND THIS IS!!!!!!!!!!!!!!!!!!!!!
"Pileup rejection DISABLED",#           15
"Inverse Pileup DISABLED",#             16
"-",#                                   17
"SHE Trace Capture DISABLED",#          18
"-",#                                   19
"-",#                                   20
"External Timestamping DISABLED",#      21
"External Fast Trigger DISABLED",#      22
"Override Peak Sample Point DISABLED"#  23
]

#Set up some global variables
BUTTON_FONT = 'Helvetica 8'
TITLE_FONT = 'Helvetica 12'
BUTTON_SIZE = (18,2)
SEP = "________________________________________________________________"
SEPARATE = "\n________________________________________________________________\n"

window = None
#txt = None

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

def read_file(channels,modules,line):
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
    function to read in all data and and return it as a list of lists of instances of Channel and a list of instances of Module. The list of lists for
    instances of the channel class has the indexing [module][channel]
    """
    input_file = open(file_path,"r")
    
    lines = input_file.readlines()
    

    no_channels = check_no_channels(lines)
    no_modules = no_channels//16

    channels = []
    modules = []
    for moduleNo in range(no_modules):
        currentModule = []
        for channelsNo in range(16):
            currentModule.append(Channel(moduleNo,channelsNo))
        channels.append(currentModule)
        modules.append(Module(moduleNo))


    for i in range(len(lines)):
        read_file(channels,modules,lines[i])

    return no_channels,no_modules,channels,modules


def output_channel_param(channels,param,no_channels,no_modules):
    """
	function which formats the data to be written out to the textbox for the user to see.

	if statement checks the parameter is a valid key. If it is not, a warning/error message will be printed to the terminal.
	This should only happen if the text file read in has been edited to change the name/format
	"""
    no_seperators = 0
    if param in channels[0][0].parameters.keys():
        ret_str = f"{param}\n\n"
        for mod in range(no_modules):
            ret_str+=f"\t\tModule {mod}"
        for chan in range(16):
            ret_str+=f"\n{chan}"
            for mod in range(no_modules):
                ret_str+=f"\t\t{channels[mod][chan].parameters[param]}"
            if chan%4 ==0 and chan>0:
                ret_str+=SEPARATE
                no_seperators+=1
        return ret_str,no_seperators
    else:
        print(f"ERROR!!!!\nKEY NOT RECOGNISED\nYOU GAVE {param}")

def compare_channel_param(channels,channels2,param,txt,no_modules):
    no_seperators = 0
    if param in channels[0][0].parameters.keys():
        txt.insert("end",f"{param}\n\n")
        for mod in range(no_modules):
            txt.insert("end",f"\t\tModule {mod}")
        for chan in range(16):
            txt.insert("end",f"\n{chan}")
            for mod in range(no_modules):
                if np.abs(channels[mod][chan].parameters[param] - channels2[mod][chan].parameters[param])<1e-6:
                    txt.insert("end",f"\t\t{channels[mod][chan].parameters[param]}","param_same")
                else:
                    txt.insert("end",f"\t\t{channels[mod][chan].parameters[param]},{channels[mod][chan].parameters[param]}","param_diff")
            if chan%4 ==0 and chan>0:
                txt.insert("end",SEPARATE)
                no_seperators+=1
    else:
        print(f"ERROR!!!!\nKEY NOT RECOGNISED\nYOU GAVE {param}")

def output_module_param(modules,param,no_modules):
    if param in modules[0].parameters.keys():
        ret_str = f"{param}\n\n"
        for mod in range(no_modules):
            ret_str+=f"\t\tModule {mod}"
        ret_str+="\n"        
        for mod in range(no_modules):
            ret_str+=f"\t\t{modules[mod].parameters[param]}"
            
        return ret_str

    else:
        print(f"ERROR!!!!\nKEY NOT RECOGNISED\nYOU GAVE {param}")

def write_txt(text,channels,modules):
    pass


def csra_test(csra_val,txt):
    try:
        print(csra_val)
        csra_val = int(csra_val)
    except ValueError:
        message = f"Value error!!! Could not convert {csra_val} to an integer!"
        txt.insert("end",message,"warning")
        return None
    if csra_val>8388607 or csra_val<0:
        message = f"Value out of range! Please make sure you entered a positive integer less than 8388607 (the maximum value with 23 bits!)"
        txt.insert("end",message,"warning")
        return None

    bit_vals = []
    binary = format(csra_val, "023b")
    for i in range(len(binary)):
        bit_vals.append(bool(int(binary[i])))
    bit_vals.reverse()
    txt.insert("end",f"CSRA test for {csra_val}\n")
    txt.insert("end","\t\tBit #\t\tValue\t\tOn?\t\tTotal\t\tFunction of bit in current state\n")
    total =0
    output_style = ""
    for i in range(len(binary)):
        
        bit_func = ""
        if bit_vals[i]:
            bit_func = CSRA_TRUE[i]
            output_style = "CSRA_on"
            total+=2**i
        else:
            bit_func = CSRA_FALSE[i]
            output_style = "CSRA_off"


        txt.insert("end", f"\t\t{i}\t\t{2**i}\t\t{int(bit_vals[i])}\t\t{total}\t\t{bit_func}\n",output_style)

    
    return None

def calc_csra(screen_text,txt):
    lines = screen_text.split('\n')[2:]
    csra_val = 0
    try:
        for i in range(len(lines)):
        
            bit_val = int(lines[i].split('\t\t')[3])
            if bit_val ==1:
                csra_val+=2**i
            elif bit_val==0:
                pass
            else:
                raise ValueError
            
    except ValueError:
        return -1
    else:
        print(csra_val)
        return csra_val
    


def write_param(param_name,screen_text,channels,modules,no_channels,no_modules,is_channel):
        if is_channel:
            lines = screen_text.split("\n")[3:]
            channel = 0
            print(f"{no_channels},{no_modules}")
            for line in lines:
                line =line.split("\t\t")[1:]
                if len(line)>3:
                    for module,value in enumerate(line):
                        old = channels[module][channel].parameters[param_name] +1e-256
                        channels[module][channel].parameters[param_name] = float(value)
                        #print(f"{channel},{module}, {old}, {channels[module][channel].parameters[param_name]}")
                    channel+=1
                    
            return channels
        
        
        else:
            lines = screen_text.split("\n")[3:]
            channel = 0
            print(f"{no_channels},{no_modules}")
            for line in lines:
                line =line.split("\t\t")[1:]
                if len(line)>3:
                    for module,value in enumerate(line):
                        old = modules[module].parameters[param_name] +1e-256
                        modules[module].parameters[param_name] = float(value)
                        print(f"{channel},{module}, {old}, {channels[module][channel].parameters[param_name]}")
                    
                    
            return modules
                    
def write_channel(channels,no_modules,no_channels,param):
    window['output'].update("Oi! This is an experimental feature - hands off!!! Wait until after the experiment before you start messing around with this!")
    
    for module in range(no_modules):
        for channel in range(16):
            value = channels[module][channel].parameters[param]
            poll_cmd = f'tmus send-keys -t poll2:1.0\\"pwrite {module} {channel} {param} {value}\\" && sleep 1 && tmux capture-pane -pt poll2:1.0'
            subprocess.run(['/bin/bash', '-i', '-c', poll_cmd])


def compare(channels,modules,filepath):
        no_channels2,no_modules2,channels2,modules2 = setup(filepath)




def main():
    ###########################################################
    ###########################################################
    ###########Create some variables used throughout###########
    ###########################################################
    ###########################################################
    file_loaded = False
    no_channels=-1
    no_modules=-1
    channels=[]
    modules=[]
    
    compare = False
    channels2=[]
    modules2=[]
    no_channels2=-1
    no_modules2=-1

    param_name = CHANNEL_PARAMS[0]
    is_channel = True
    no_seperators=0
    sg.theme('Light Blue 2')
    '''
    button_line lists store the buttons that the user can click on to see parameters
    lines 1,2,3,4 have channel parameters while 5 and 6 have module parameters
    '''
    button_line1 = []
    button_line2 =[]
    button_line3 =[]
    button_line4=[]
    
    button_line5=[]
    button_line6=[]
    
    #############################################################################
    #############################################################################
    ############################CREATE SCREEN LAYOUT#############################
    #############################################################################
    #############################################################################


    for i in range(11):
        try:
            button_line1.append(sg.Button(CHANNEL_PARAMS[i],button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE))
            button_line2.append(sg.Button(CHANNEL_PARAMS[i+11],button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE))
            button_line3.append(sg.Button(CHANNEL_PARAMS[i+22],button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE))
            button_line4.append(sg.Button(CHANNEL_PARAMS[i+33],button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE))
        except IndexError:
            '''
            bad coding, I know.... Easiest way to make the code loop through the buttons names
            '''
            pass
        try:
            button_line5.append(sg.Button(MODULE_PARAMS[i],button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE))
            button_line6.append(sg.Button(MODULE_PARAMS[i+11],button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE))
        except IndexError:
            pass
    '''
    layout list defines what the screen layout is. ie. which buttons/other features are where.
    It is a 2D list where the first index gives the line (top to bottom) and the second index gives
    the horizontal position (left to right)
    
    '''
    #############################################################################
    #############################################################################
    ############################DEFINE SCREEN LAYOUT#############################
    #############################################################################
    #############################################################################

    layout = [
            [sg.Text('config file path', size=(15, 1),font=TITLE_FONT), 
            sg.Input(default_text = "C:/Users/User/Documents/PhD/Sort Code/Besbin/scripts/IS659_4May2022.txt", key='file'), 
            sg.FileBrowse(initial_folder = "C:"),
            sg.Button("Load",button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE),
            sg.Button("Compare",button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE),
            sg.Button("Overwrite",button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE),
            sg.Exit(font=BUTTON_FONT, size=(4,2))],
            
            [sg.Text('Individual Channel parameters:', size=(24, 1),font=TITLE_FONT)],
            button_line1,
            button_line2,
            button_line3,
            button_line4,
            [sg.Text("Full module parameters:", size=(20, 1),font=TITLE_FONT)],
            button_line5,
            button_line6,
            [sg.Multiline(size=(250,28), autoscroll=True, key='output',font='Helvetica 8')], 
            [sg.Input(default_text = "1028", key='csra'),
            sg.Button("CSRA test",button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE),
            sg.Button("Calc CSRA",button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE),
            sg.Button("Dump and load",button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE),
            ]
            ]
    
    
    #create the instance of the sg.Window class with the layout 'layout'
    
    
    window = sg.Window('IDS Pixie parameters', layout)
    


    while True:  
        """
        This while loop is the main loop for the GUI.
        While running, sg will check which buttons are read using window.read()
        Because no file is loaded by default, the load and exit buttons are the only ones which will 
        do anything at the start.
        Once a file is loaded, file_loaded is set to True and the user may use the other buttons to check parameters

        """

        
        event, values = window.read()
        
        txt = window['output'].Widget

        txt.tag_config("CSRA_on",background = "white",foreground="darkgreen")
        txt.tag_config("params_same",background = "white",foreground="darkgreen")
        txt.tag_config("params_diff",background = "white",foreground="red")
        txt.tag_config("CSRA_off",background = "white",foreground="red")
        txt.tag_config("warning", background="yellow", foreground="red")
        
        
        ###################################################################################################
        """
        These buttons can be pressed/used before a file is loaded....
        """
        ###################################################################################################
        if event == sg.WIN_CLOSED or event == 'Exit':
                break
            
        if event =='Load':
            try:
                no_channels,no_modules,channels,modules = setup(values['file'])
            except FileNotFoundError:
                window['output'].update("Error, the file could not be found. Please check path!")
            except KeyError:
                window['output'].update("Warning - there was an issue with the parameter names. Please check terminal")
            else:
                window['output'].update("File loaded successfully")
                file_loaded = True
        if event =="Dump and load":
                           
            exp_no = os.getenv('CURRENT_EXP').split("/")[-1]
            current_time = str(datetime.now()).split(".")[0].replace(" ","_").replace(":","-")
            dump_name = f"{exp_no}_{current_time}"
            subprocess.run(['/bin/bash', '-i', '-c', f'"tmux send-keys -t poll2:1.0 \"dump {dump_name}\" Enter && sleep 1 && tmux capture-pane -pt poll2:1.0"'])
            time.sleep(1)
            no_channels,no_modules,channels,modules = setup(file_path=f"/home/pixie16/poll/{dump_name}")
            try:
                no_channels,no_modules,channels,modules = setup(values['file'])
            except FileNotFoundError:
                window['output'].update("Error, the file could not be found. Please check path")
            except KeyError:
                window['output'].update("Warning - there was an issue with the parameter names. Please check terminal")
            else:
                window['output'].update(f"current parameters dumped to {values['dump_path']}.File loaded successfully")
            
        
        if event =="CSRA test":
            window['output'].update("")
            csra_test(values['csra'],txt)

        if event == "Calc CSRA":
            csra_val = calc_csra(values['output'],txt)
            window['csra'].update(csra_val)
            window['output'].update("")
            values['csra'] = csra_val
            csra_test(values['csra'],txt)


        ###################################################################################################
        """
        These buttons require a file to be loaded to be used
        """
        ###################################################################################################

        if file_loaded:
            if event in CHANNEL_PARAMS:
                if not compare:
                    string_to_write,no_seperators = output_channel_param(channels,event,no_channels,no_modules)
                    window['output'].update(string_to_write)
                    is_channel = True
                    param_name = event
                    
                else:
                    print("comparing!")
                    window['output'].update("")
                    compare_channel_param(channels,channels2,event,txt,no_modules)
                    is_channel = True
                    param_name = event
            
            if event in MODULE_PARAMS:
                string_to_write = output_module_param(modules,event,no_modules)
                window['output'].update(string_to_write)
                is_channel = False
                param_name = event
            
            
            if event == "write txt":
                write_txt(values,channels,modules)

            if event =="Overwrite":
                print(param_name)
                if is_channel:
                    channels = write_param(param_name,values['output'],channels,modules,no_channels,no_modules,True)
                else:
                    modules =  write_param(param_name,values['output'],channels,modules,no_channels,no_modules,False)
        
            if event =="Compare":
                no_channels2,no_modules2,channels2,modules2 = setup(values['file'])
                compare=True


        
        else:
            if event == sg.WIN_CLOSED or event == 'Exit':
                break

            if event in CHANNEL_PARAMS or event in MODULE_PARAMS or event =="dump" or event == "write txt"or event == "Overwrite":
                window['output'].update("\n\n\t\tNO FILE LOADED. PLEASE SELECT A FILE ABOVE!!!")
                

			
    window.close()
    
    

main()

