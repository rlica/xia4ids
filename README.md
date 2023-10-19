# xia4ids
 XIA DGF Pixie-16 .ldf data file converter into ROOT and GASPware format for IDS (The IDS Collaboration https://isolde-ids.web.cern.ch/ ) 

Current version: Jakku (October 2023)

Parts of this code were modified from Pixie Acquisition and Analysis Software Suite (PAASS) https://github.com/pixie16/paass, which is licensed under the GNU GPL v. 3.0. In particular, some classes are modified and adapted from PAASS of branch 'dev' at https://github.com/pixie16/paass/tree/dev/Analysis/ScanLibraries.



## Authors and contributors
 * Razvan Lica, CERN / IFIN-HH, razvan.lica@cern.ch
 * Khai Phan, Tampere University - CERN Summer Student Project 2020, khai.phan@tuni.fi
 * James Cubiss, University of York, james.cubiss@york.ac.uk
 * Chris Page, University of York, chris.page@york.ac.uk
 * Andres Illana, Universidad Complutense Madrid, andres.illana@ucm.es


## Installation and running
 1. Download via github.com or using the command line  
 `git clone https://github.com/rlica/xia4ids`
 2. Compile with `make`.
 3. [optional] Add in `$HOME/.bashrc` or `$HOME/.profile`       
 `PATH=$PATH:/your_path_here/xia4ids/bin/`
 4. Run: `xia4ids config_file calibrationFile[optional]`
 5. Enjoy!


## Getting Started
### The 'xia4ids' converter requires a configuration file in which the user specifies:
 * runName  = location of raw data including the name of the runs, not including the numbers
 * timegate = coincidence timegate
 * cs-tac, pair-tac, flagtype = specific detector types (only for GASPware - for ROOT set them to zero)
 * reftype  = the reference type (proton pulse)
 * ref_unit  = the unit for the event time versus the reference (Low Resolution Time)
 * run_unit  = the unit for the timestamp
 * Format = the format in which to convert the data 
      * gasp = GASPware
      * root = ROOT
      * list = binary event lists
      * stat = print statistics for the entire run, the event builder will be skipped
      * rate = only print statistics for the last buffer, the event builder will be skipped
 * Fold = the number of coincidences that will trigger an event 
 * The detector configuration depending on the selected output format (see below).
 * Correlation mode (optional) - the program will skip the event builder and will only histogram the time
 differences between the selected modules and channels. 

IMPORTANT: The number of modules and their configuration is hard-coded. Edit 'unpacker.cpp' from line 236.

See the 'etc' folder for some examples of different configuration and calibration files.

To test the code, a raw data file from 152Eu source and configuration file cand be downloaded from: 
https://cernbox.cern.ch/index.php/s/1C5pXrtTSCdneZm

#### ROOT format:

The .root file will contain an event tree and singles histograms for all the selected inputs.  
These are the following branches in the tree:

* E_type1[ ] - ADC data
* E_type2[ ]
* .
* .
* T_type1[ ] - (HRT) timestamp difference between each branch and the beginning of the event with offset = 1000
* T_type2[ ]
* .
* .  
* M_type1[ ] - multiplicity for each branch
* M_type2[ ]
* .
* . 
* MULT     - the multiplicity of the event (fold)
* TIME     - timestamp difference between the event and the reference (proton pulse or run start) 
* TIME_RUN - timestamp of the event in `run_unit` units 


1. The detectors are defined in the config file. The 'Comments' column will be used for branch naming. 
2. An individual index-type combination represents a detector. Each distinct type represents a branch in the tree. 
The index corresponds to the leaf number (because each branch is an array).  
3. There can be several detectors of the same type and index. In case they are in the same event, their energies will be
summed up in the corresponding leaf. A calibration file should be provided.
4. There will be at least 'fold' detectors in the event. 


#### GASPware format:

1. The event will contain one Header and the detectors defined in the config file (index and detector type).
2. Each detector will have at least three parameters:
    * Energy
    * HRT(High Resolution Time) - the time difference between each signal and the time centroid of the event
    * LRT(Low Resolution Time)  - the time difference between the event and the reference signal (refType).
    * If there is no reference defined (refType 0), the LRT parameter will be missing.
3. 'pair-tac' will be treated as a detector with two extra parameters: the start and stop energies.   
    * At 'Link' set the detector type that starts and stops the pair-tac (eg LaBr)
    * At 'Start' and 'Stop' set the Index of the 'Link' that starts and stops the tac
4. 'cs-tac' is set between each detector and the common stop/start.
    * It will be considered as a parameter for the detector type set at 'Link'
5. The header will contain the timestamp of the event in `run_unit` units.


#### LIST format:

1. The event consists of several entries, written as 32bit integers such as:
     * (proton-time)-(beta-energy)-(labr-energy)-(TAC_Beta-LaBr)
     * (proton-time)-(beta-energy)-(labr1-energy)-(TAC_Beta_Labr1)-(Labr2-energy)-(TAC_labr1_labr2)

2. There is a 'zero' separator before each event.

3. Each distinct type represents an entry in the event. 

4. There can be several detectors of the same type. They should be calibrated because they go in the same entry.
The type ordering in the config file is the ordering of the entries in the event.

5. To write the timestamp difference between current entry and a common reference
inside the event, set the type of the reference at "Link" for each detector. 
The timestamp difference entry will appear after the energy entry.

6. To write the event time versus the reference signal (proton-time), set the referece at refType.

#### RATE format:

1. Set the input file as the second argument: `xia4ids [config_file] [input_file] [calibration_file]`.
2. It will print the statistics for the last buffer in the input file. You will be asked to input the number
of blocks you want to be read from the end of the file. 
3. You will be asked if you want to output the results in a ROOT file (useful for real-time yield measurement).
4. Use the `macros/xia4ids_rate.sh` script file in order to get automatically the last file in a folder.

