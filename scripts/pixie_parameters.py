#!/usr/bin/env python3

#Simple XIA Pixie-16 DAQ GUI
#poll2 needs to be running in tmux
#
#C. Page 2022
#Last change 23/09/2022

#C Page finally added graphing
#Prerequisites:
#pip3 install pysimplegui
#yum install python3-tkinter

#Direct interaction with the pixie requires tmux session to be initialised in the normal way

#Add the following lines in the .bashrc file:
##### Control poll2 from outside the tmux session (assuming session name 'poll2' and poll2 is started in pane 1.0)


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
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg


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
no_chn_params = len(CHANNEL_PARAMS)
no_mod_params = len(MODULE_PARAMS)

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
"HIGH gain",#                            14 #############CHECK WHICH WAY AROUND THIS IS!!!!!!!!!!!!!!!!!!!!!
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
"LOW gain",#                           14 #############CHECK WHICH WAY AROUND THIS IS!!!!!!!!!!!!!!!!!!!!!
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

    def __init__(self,module,channel,dt):
        self.module = module
        self.channel = channel
        self.parameters = {}
        self.param_changed = {}
        self.dt =dt
        for param in CHANNEL_PARAMS:
            self.parameters[param] = -np.inf
            self.param_changed[param] = False


    def __str__(self):
        return f"{self.module}.{self.channel}"
class Module():

    def __init__(self,module,dt):
        self.module = module
        self.parameters = {}
        self.param_changed = {}
        self.dt =dt
        for param in MODULE_PARAMS:
            self.parameters[param] = -np.inf
            self.param_changed[param] = False
class Run():

    def __init__(self,channels,modules,no_channels,no_modules,run_number,dt):
        self.channels = channels
        self.modules = modules
        self.number = -1
        try:
            self.number = int(run_number)
        except ValueError:
            print(f"error with writing run number. Setting to -1\n")
        self.no_channels = no_channels
        self.no_modules = no_modules
        self.dt = dt

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

def get_run_number():
        FOLDER = os.getenv('CURRENT_EXP')
        list_of_files = glob.glob(FOLDER+'/RAW/*.ldf') #Note: always update the $CURRENT_EXP env variable in .bashrc
        latest_file = max(list_of_files, key=os.path.getctime)
        return int(re.search('run_(\d+)', latest_file).group(1))

def get_exp_number():
    return os.getenv('CURRENT_EXP').split("/")[-1]


def setup(file_path,plotting=False,dt = datetime.fromtimestamp(0) ):
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
            currentModule.append(Channel(moduleNo,channelsNo,dt))
        channels.append(currentModule)
        modules.append(Module(moduleNo,dt))


    for i in range(len(lines)):
        read_file(channels,modules,lines[i])
    if plotting:
        print(file_path)
        run_number =  int(file_path.split("run_")[1].split(".txt")[0])
        print(run_number)
        return Run(channels,modules,no_channels,no_modules,run_number,dt)
    return no_channels,no_modules,channels,modules


def output_channel_param(channels,param,no_channels,no_modules):
    """
	function which formats the data to be written out to the textbox for the user to see.

	if statement checks the parameter is a valid key. If it is not, a warning/error message will be printed to the terminal.
	This should only happen if the text file read in has been edited to change the name/format
	"""
    if param!="CHANNEL_CSRB":
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
    elif param=="CHANNEL_CSRB":
        no_seperators = 0
        if param in channels[0][0].parameters.keys():
            ret_str = f"{param}\n\n"
            for mod in range(no_modules):
                ret_str+=f"\t\tModule {mod}"
            for chan in range(16):
                ret_str+=f"\n{chan}"
                for mod in range(no_modules):
                    ret_str+=f"\t\t{str(channels[mod][chan])}"
                if chan%4 ==0 and chan>0:
                    ret_str+=SEPARATE
                    no_seperators+=1
            return ret_str,no_seperators
    else:
        print(f"ERROR!!!!\nKEY NOT RECOGNISED\nYOU GAVE {param}")

def compare_channel_param(channels,channels2,param,txt,no_modules,f1_name,f2_name):
    no_seperators = 0
    txt.insert("end",f"Comparing {f1_name} with {f2_name} {param}\n\n")

    if param in channels[0][0].parameters.keys():
        
        for mod in range(no_modules):
            txt.insert("end",f"\t\tModule {mod}")
        for chan in range(16):
            txt.insert("end",f"\n{chan}")
            for mod in range(no_modules):
                if np.abs(channels[mod][chan].parameters[param] - channels2[mod][chan].parameters[param])<1e-6:
                    txt.insert("end",f"\t\t{channels[mod][chan].parameters[param]}","params_same")
                else:
                    txt.insert("end",f"\t\t{channels[mod][chan].parameters[param]}  /  {channels2[mod][chan].parameters[param]}","params_diff")
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
    lines = screen_text.split('\n')[1:]
    csra_val = 0
    try:
        for i in range(len(lines)-2):
            print(lines[i])

            asd = lines[i].split('\t\t')
            try:
                bit_val = int(asd[3])
            except IndexError:
                print(f"{i} , {asd}")
            if bit_val ==1:
                csra_val+=2**i
            elif bit_val==0:
                pass
            else:
                raise ValueError
            
    except ValueError:
        return -1
    else:
        #print(csra_val)
        return csra_val
    


def store_param(param_name,screen_text,channels,modules,no_channels,no_modules,is_channel):
        if is_channel:
            lines = screen_text.split("\n")[3:]
            channel = 0
            
            for line in lines:
                line =line.split("\t\t")
                try:
                    channel = int(line[0])
                except ValueError:
                    pass
                else:
                    line = line[1:]
                    if len(line)>3:
                        for module,value in enumerate(line):
                            #print(f"{module}    {channel}     {param_name}    {value}")
                            try:
                                old = channels[module][channel].parameters[param_name]
                                if np.abs(float(value) - old) >1e-7:
                                    print("changing")
                                    channels[module][channel].parameters[param_name] = float(value)
                                channels[module][channel].param_changed[param_name] = True
                            except IndexError:
                                print(f"index error for {module}    {channel}")
                            except ValueError:
                                print(f"Value error for {module}    {channel}     {value}")
                    
            return channels
        
        
        else:
            lines = screen_text.split("\n")[3:]
            channel = 0
            print(f"{no_channels},{no_modules}")
            for line in lines:
                line =line.split("\t\t")[1:]
                if len(line)>3:
                    for module,value in enumerate(line):
                        old = modules[module].parameters[param_name]
                        if np.abs(float(value) - old) <1e-7:
                            modules[module].parameters[param_name] = float(value)
                            modules[module].param_changed[param_name] = True

                    
                    
            return modules
                    
def update_pixie(channels,modules,no_modules,no_channels,force_update=False):
    try:
        start_time = str(datetime.now()).split(".")[0].replace(" ","_").replace(":","-")
        sucessful = 0
        failed = 0 
        
        s_log_text = "\n\nLog of sucesful writting:\n"
        f_log_text = "\n\nLog of failed writting:\n"
        
        for param_num,param in enumerate(CHANNEL_PARAMS):
            
            for module in range(no_modules):
                for channel in range(16):
                    try:
                        if(channels[module][channel].param_changed[param] or force_update) :
                            value = channels[module][channel].parameters[param]
                            poll_cmd = f'tmux send-keys -t poll2:1.0 \"pwrite {module} {channel} {param} {value}\" Enter && tmux capture-pane -pt poll2:1.0'
                            result = subprocess.run(['/bin/bash', '-i', '-c', poll_cmd], stdout=subprocess.PIPE).stdout.decode("utf-8")
                            lines = result.split("\n")
                            
                            
                            
                            if lines[-4][-3:-1]=="OK" or lines[-3][-3:-1]=="OK" or lines[-5][-3:-1]=="OK":
                                sucessful+=1
                                s_log_text+=f"\n success number {sucessful}\n{result[-6:-3]}\n{SEPARATE}n"
                            else:
                                print("failed on\n")
                                print(lines[-5:] )
                                failed+=1
                                f_log_text+=f"\n failure number {failed}: {poll_cmd}\n{result[-6:-3]}\n{SEPARATE}n"
                                return None
       
                    except IndexError:
                        failed +=1 
                        s_log_text+=f"\nPYTHON INDEX ERROR!!! failure number {failed}\n{result[-6:-3]}\n{SEPARATE}\n"
              
            
        for param in MODULE_PARAMS:
            for module in range(no_modules):
                try:
                    if modules[module].param_changed[param] or force_update:
                        value = modules[module].parameters[param]
                        poll_cmd = f'tmux send-keys -t poll2:1.0 \"pmwrite {module} {param} {int(value)}\" Enter && tmux capture-pane -pt poll2:1.0'
                        result = subprocess.run(['/bin/bash', '-i', '-c', poll_cmd], stdout=subprocess.PIPE).stdout.decode("utf-8")
                        lines = result.split("\n")
                        if lines[-4][-3:-1]=="OK" or lines[-3][-3:-1]=="OK" or lines[-5][-3:-1]=="OK":
                            sucessful+=1
                            s_log_text+=f"\n success number {sucessful}\n{result[-6:-3]}\n{SEPARATE}n"
                        else:
                            failed+=1
                            f_log_text+=f"\n failure number {failed}\n{result[-6:-3]}\n"
                            f_log_text+=f"\n failure number {failed}: {poll_cmd}\n{result[-6:-3]}\n{SEPARATE}n"

                except IndexError:
                    failed +=1 
                    s_log_text+=f"\nPYTHON INDEX ERROR!!! failure number {failed}\n{result[-6:-3]}\n{SEPARATE}n"
        
        
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
        subprocess.run(['/bin/bash', '-i', '-c', poll_cmd], stdout=subprocess.PIPE).stdout.decode("utf-8")
    except KeyboardInterrupt:
        pass
    




def main():
    ###########################################################
    ###########################################################
    ###########Create some variables used throughout###########
    ###########################################################
    ###########################################################
    file_loaded = False
    compare = False
    plotting = False
    writing = False

    no_channels=-1
    no_modules=-1
    channels=[]
    modules=[]
    

    all_runs=[]
    plotting_module = 0
    changed_module = False
    last_plot = "TAU"
    to_plot = "TAU"
    plot_run_no = True
    plot_disabled = True

    channels2=[]
    modules2=[]
    no_channels2=-1
    no_modules2=-1
    f1_name = ''
    f2_name = ''

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
    plot_menu = ["0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15"]
    layout = [
            [sg.Text('config file path', size=(15, 1),font=TITLE_FONT), 
            sg.Input(default_text = "/home/pixie16/poll", key='file'), 
            sg.FileBrowse(initial_folder = "/home/pixie16/poll"),
            sg.Button("Load",button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE),
            sg.Button("Compare",button_color='red', font=BUTTON_FONT, size=BUTTON_SIZE),
            sg.Button("Dump and load",button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE),
            sg.Button("Store",button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE),
            sg.Button("Send changed parameters",button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE),
            sg.Button("Send all parameters",button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE),
            sg.Exit(font=BUTTON_FONT, size=(4,2))],
            [sg.Button("Plot",button_color='red', font=BUTTON_FONT, size=BUTTON_SIZE),
            sg.Button("Plot x axis",button_color='blue', font=BUTTON_FONT, size=BUTTON_SIZE),
            sg.OptionMenu(values = plot_menu,default_value="0",key="plot menu"),
            sg.Input(default_text = "/home/pixie16/poll", key='plot folder'),
            sg.FolderBrowse(initial_folder = "/home/pixie16/poll",key="plot folder browse"),
            sg.Button("Plot disabled",button_color='red', font=BUTTON_FONT, size=BUTTON_SIZE)],
            
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
            sg.Button("Calc CSRA",button_color='sea green', font=BUTTON_FONT, size=BUTTON_SIZE)]
            ]
    
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
                f1_name = values['file'].split("/")[-1]
                file_loaded = True
        
        
        if event =="Dump and load":
                           
            exp_no = get_exp_number()

            current_time = str(datetime.now()).split(".")[0].replace(" ","_").replace(":","-")
            if not os.path.isdir(f"/home/pixie16/poll/parameters/{exp_no}"):
                print("parameter folder for current experiment not found. creating one now...")
                os.mkdir(f"/home/pixie16/poll/parameters/{exp_no}")
            run_no = get_run_number()
            dump_path = f"/home/pixie16/poll/parameters/{exp_no}/{exp_no}_{current_time}_run_{run_no}.txt"
            subprocess.run(['/bin/bash', '-i', '-c', f'tmux send-keys -t poll2:1.0 \"dump {dump_path}\" Enter && sleep 1 && tmux capture-pane -pt poll2:1.0'],check = True)

            time.sleep(1)
            
           
            try:
                no_channels,no_modules,channels,modules = setup(file_path=dump_path)
            except FileNotFoundError:
                window['output'].update(f"Error, the file could not be found. Please check path")
            except KeyError:
                window['output'].update("Warning - there was an issue with the parameter names. Please check terminal")
            else:
                window['output'].update(f"current parameters dumped to {exp_no}_{current_time}.txt. File loaded successfully")
                file_loaded = True
            
        
        if event =="CSRA test":
            window['output'].update("")
            csra_test(values['csra'],txt)

        if event == "Calc CSRA":
            csra_val = calc_csra(values['output'],txt)
            window['csra'].update(csra_val)
            window['output'].update("")
            values['csra'] = csra_val
            csra_test(values['csra'],txt)
        if event == "Plot disabled":
            plot_disabled = not plot_disabled
            if plot_disabled:
                colour = "sea green"
            else:
                colour = "red"
            window[ "Plot disabled"].update( "Plot disabled", button_color=colour)
            

        if event =="Plot":
            if plotting :
                plotting = False
                window["Plot"].update("Plot", button_color='red')
            else:     
                plotting = True
                compare = False
                window["Plot"].update("Plot", button_color='sea green')
                window["Compare"].update("Compare", button_color='red')

                plotting_dir =values['plot folder']
                exp_no = plotting_dir.split("/")[-1]
                file_names = os.listdir(plotting_dir)
                for file in file_names:
                    if file.startswith(exp_no):
                        file_time = file.replace(exp_no,"")
                        file_time = file_time.replace(".txt","")[1:20]
                        #print(file_time)
                        try:
                            dt_obj = datetime.strptime(file_time,"%Y-%m-%d_%H-%M-%S")
                        except ValueError:
                            print(f"error with generating datetime object for file {file}")
                        
                        else:

                            run = setup(f"{plotting_dir}/{file}",True,dt_obj)
                            all_runs.append(run)


        if event == "Plot x axis":
            plot_run_no = not plot_run_no
    

        if plotting and values["plot menu"]!=plotting_module:
            plotting_module = int(values["plot menu"]) 
            changed_module = True

        if plotting:
            if event in CHANNEL_PARAMS:
                if changed_module and (event=="Plot x axis" or event=="Plot"):
                    to_plot = last_plot
                else:
                    to_plot = event
                changed_module = False

                plt.clf()
                run_nos =[]
                dts = []
                for run in all_runs:
                    random_offset = np.random.uniform(-1.e-4,1.e-4)
                    dts.append(run.dt)
                    run_nos.append(random_offset + run.number)
               
                module_vals =[]
                for channel in range(16):
                    channel_vals =[]
                    for run in all_runs:
                        random_offset = np.random.uniform(0.9999,1.0001)
                        #*small* random offset added because otherwise values at the same point would not all be visible
                        if plot_disabled or run.channels[plotting_module][channel].parameters["CHANNEL_CSRA"]>0:#only plot disabled channels if plot_disabled==True
                            try:
                                channel_vals.append(random_offset*run.channels[plotting_module][channel].parameters[to_plot])
                            except IndexError:
                                window['output'].update(f"Warning - Index error when trying to plot {plotting_module} {channel} {to_plot}.\nMost likely, you are selecting a module which wasn't used! ")
                        else:
                            print(f"ignoring disabled channel M{plotting_module} {channel} {run.number}")
                    module_vals.append(channel_vals)
                    if plot_run_no:
                        plt.scatter(run_nos,channel_vals,marker='x',label=f'Channel {channel}')
                        plt.xlabel("Run number")
                    else:
                        plt.scatter(dts,channel_vals,marker='x',label=f'Channel {channel}')
                        plt.xlabel("date - time of run")
                plt.legend()
                plt.ylabel(f"{event} value")
                plt.title(f"{event} for module {plotting_module} as function of module")
                plt.show()
                if event !="Plot":
                    last_plot = event
                

        ###################################################################################################
        """
        These buttons require a file to be loaded to be used
        """
        ###################################################################################################

        if file_loaded:
            if event in CHANNEL_PARAMS:
                if not compare and not plotting:#Normal display mode
                    
                    string_to_write,no_seperators = output_channel_param(channels,event,no_channels,no_modules)
                    window['output'].update(string_to_write)
                    is_channel = True
                    param_name = event
                    
                    
                elif not plotting:#comparison mode
                    window['output'].update("")
                    compare_channel_param(channels,channels2,event,txt,no_modules,f1_name,f2_name)
                    is_channel = True
                    param_name = event


                for param in CHANNEL_PARAMS:
                    window[param].update(param, button_color='sea green')
                for param in MODULE_PARAMS:
                    window[param].update(param, button_color='sea green')
                
                
                window[event].update(event, button_color='blue')


            if event in MODULE_PARAMS:
                string_to_write = output_module_param(modules,event,no_modules)
                window['output'].update(string_to_write)
                is_channel = False
                param_name = event
                for param in CHANNEL_PARAMS:
                        window[param].update(param, button_color='sea green')
                for param in MODULE_PARAMS:
                    window[param].update(param, button_color='sea green')
                    
                window[event].update(event, button_color='blue')
            
            
            if event == "write txt":
                write_txt(values,channels,modules)

            if event =="Store":
                if is_channel:
                    channels = store_param(param_name,values['output'],channels,modules,no_channels,no_modules,True)
                else:
                    modules =  store_param(param_name,values['output'],channels,modules,no_channels,no_modules,False)

            if event =="Send changed parameters":
                update_pixie(channels,modules,no_modules,no_channels,False)
            if event =="Send all parameters":
                update_pixie(channels,modules,no_modules,no_channels,True)

            if event =="Compare":
                if not compare:
                    no_channels2,no_modules2,channels2,modules2 = setup(values['file'],False)
                    f2_name = values['file'].split("/")[-1]
                    window['output'].update("\n\n\t\tFile loaded succesfully for comparison")
                    window[event].update(event, button_color='sea green')
                    
                    compare=True
                    plotting = False
                else:
                    window[event].update(event, button_color='red')
                    compare = False
                    plotting = False

        
        else:
            if event == sg.WIN_CLOSED or event == 'Exit':
                break

            if event in CHANNEL_PARAMS or event in MODULE_PARAMS or event =="dump" or event == "write txt"or event == "Store":
                window['output'].update("\n\n\t\tNO FILE LOADED. PLEASE SELECT A FILE ABOVE!!!")
                

			
    window.close()
    


main()



