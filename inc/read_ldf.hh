// Author: Khai Phan, TUNI-CERN Summer Student, 2020
// Updated by Razvan Lica, 2021

//.ldf file structure:
// word (1 byte) -> buffer (8194 words) -> chunk (Head + Dir + Data buffers)-> spill -> file -> run (one file or multiple 2Gb files)

#include <iostream>
#include <sstream>
#include <vector>
#include <utility>

#include "LDFReader.h"
#include "XiaData.h"
#include "Unpacker.h"

#define binary_file ldf.GetFile() /// Main input file (binary file)
#define file_length ldf.GetFileLength() /// Main input file length (in bytes).
//#define curr_buffer data.GetCurrBuffer()
//#define next_buffer data.GetNextBuffer()
//#define buff_pos data.GetBuffPos()

#define HEAD 1145128264 /// Run begin buffer
#define DATA 1096040772 /// Physics data buffer
#define DIR 542263620   /// "DIR "
#define LDF_DIR_LENGTH 8192 /// Size of DIR buffer
#define LDF_DATA_LENGTH 8193 /// Maximum length of an ldf style DATA buffer.
#define ENDFILE 541478725 /// End of file buffer
#define ENDBUFF 0xFFFFFFFF /// End of buffer marker
#define ACTUAL_BUFF_SIZE 8194


int read_ldf(LDF_file& ldf, DATA_buffer& data, int& pos_index) {
    DIR_buffer dir;
    HEAD_buffer head;
   
    std::vector<XiaData*> decodedList_; /// The main object that contains all the decoded signals.

    unsigned long num_spills_recvd = 0; /// The total number of good spills received from either the input file or shared memory.
    bool debug_mode = DEBUG; /// Set to true if the user wishes to display debug information.
    bool is_verbose = VERBOSE; /// Set to true if the user wishes verbose information to be displayed.

    // variables for reading dir buffer
    char* x = new char[2];
    unsigned int check_bufftype_dir, check_buffsize_dir, unknown[2], run_num;

    // variables for reading head buffer
    unsigned int check_bufftype_head, check_buffsize_head, run_num_2;
    char facility[9];
    char format[9];
    char type[17];
    char date[17];
    char run_title[81];

    // variables for reading data buffer
    bool full_spill;
    bool bad_spill;
    unsigned int nBytes;
    std::stringstream status;

    // variable that stores data spill
    unsigned int* data_ = new unsigned int[250000];

    // variable for decoding the data spill
    Unpacker unpacker_; /// Pointer to class derived from Unpacker class.

    binary_file.open(ldf.GetName().c_str(), std::ios::binary);

    if (!binary_file.is_open() || !binary_file.good()) {
        if (!binary_file.is_open()) { std::cout << "binary file cannot be opened."; }
        std::cout << " ERROR! Failed to open input file '" << ldf.GetName() << "'! Check that the path is correct.\n";
        binary_file.close();
        data.SetRetVal(6);
        return -1;
    }

    //std::cout << "Enter the maximum number of spills to parse (type 0 to read all): ";
    //std::cin >> max_num_spill;
        
    if (max_num_spill != 0) {
        if (debug_mode)
            std::cout << "Maximum number of spills to read per cycle: " << max_num_spill << std::endl;
    }
    else
        std::cout << "No maximum number of spills specified, proceed to read all spills!" << std::endl;
    // Get length and rewind to read from beg
    binary_file.seekg(0, binary_file.beg);

    //// Start reading ldf DIR buffer
    //// First we check type and size
    //binary_file.read((char*) &check_bufftype_dir, 4);
    //binary_file.read((char*) &check_buffsize_dir, 4);
    //if (check_bufftype_dir != dir.GetBufferType() ||
    //    check_buffsize_dir != dir.GetBufferSize()) { // Not a valid DIR buffer
    //    binary_file.seekg(-8, binary_file.cur); // Rewind to the beginning of this buffer
    //    std::cout << "Not a valid HEAD buffer, rewinded and returned -1";
    //    return -1;
    //}
    //// First buffer is indeed DIR, proceed
    //binary_file.read((char*) unknown, 8);
    //dir.SetUnknown(unknown);
    //binary_file.read((char*) &run_num, 4);
    //dir.SetRunNum(run_num);
    //binary_file.seekg(((unsigned long long) check_buffsize_dir * 4 - 20), binary_file.cur); // Skip the rest of the buffer
    //std::cout << "check_bufftype_dir: " << check_bufftype_dir << std::endl;
    //std::cout << "check_buffsize_dir: " << check_buffsize_dir << std::endl;
    //std::cout << "unknown dummy: " << *unknown << std::endl;
    //std::cout << "unknown: " << *dir.GetUnknown() << std::endl;
    //std::cout << "run_num dummy: " << run_num << std::endl;
    //std::cout << "run_num: " << dir.GetRunNum() << std::endl;





    //// Start reading ldf HEAD buffer
    //binary_file.read((char*)&check_bufftype_head, 4);
    //binary_file.read((char*)&check_buffsize_head, 4);
    //if (check_bufftype_head != head.GetBufferType() ||
    //    check_buffsize_head != head.GetBufferSize()) { // Not a valid HEAD buffer
    //    binary_file.seekg(-8, binary_file.cur); // Rewind to the beginning of this buffer
    //    std::cout << "Not a valid HEAD buffer, rewinded to beginning of buffer and returned -1";
    //    //return -1;
    //}

    //binary_file.read(facility, 8);
    //facility[8] = '\0';
    //binary_file.read(format, 8);
    //format[8] = '\0';
    //binary_file.read(type, 16);
    //type[16] = '\0';
    //binary_file.read(date, 16);
    //date[16] = '\0';
    //binary_file.read(run_title, 80);
    //run_title[80] = '\0';
    //binary_file.read((char*)&run_num_2, 4);
    //binary_file.seekg((ACTUAL_BUFF_SIZE * 4 - 140),
    //    binary_file.cur); // Skip the rest of the buffer
    //std::cout << "index after dir is: " << binary_file.tellg() << std::endl;
    //std::cout << "head buffer type is: " << check_bufftype_head;





    // Start reading ldf DATA buffers
    if (pos_index == 0)
    {
        binary_file.seekg(65552, binary_file.beg); //10010 (hex) offset of DATA buffer type
    }
    else
    {
        binary_file.seekg(pos_index, binary_file.beg); // resume reading the following spills
    }
    

    // data.Reset();

    while (true) {
        if (!data.Read(&binary_file, (char*)data_, nBytes, 0, full_spill, bad_spill, debug_mode)) {     // Reading a spill from the binary file
			
			
            if (data.GetRetval() == 1) {
                if (debug_mode) {
                    std::cout << "debug: Encountered single EOF buffer (end of run).\n";
                }
            }
            else if (data.GetRetval() == 2) {
                if (debug_mode) {
                    std::cout << "debug: Encountered double EOF buffer (end of file).\n";
                }
                break;
            }
            else if (data.GetRetval() == 3) {
                if (debug_mode) {
                    std::cout << "debug: Encountered unknown ldf buffer type.\n";
                }
            }
            else if (data.GetRetval() == 4) {
                if (debug_mode) {
                    std::cout << "debug: Encountered invalid spill chunk.\n";
                }
            }
            else if (data.GetRetval() == 5) {
                if (debug_mode) {
                    std::cout << "debug: Received bad spill footer size.\n";
                }
            }
            else if (data.GetRetval() == 6) {
                if (debug_mode) {
                    std::cout << "debug: Failed to read buffer from input file.\n";
                }
                break;
            }
            continue;
        }

        if (debug_mode) {
            status << "\033[0;32m" << " [READ] " << "\033[0m" << nBytes / 4 << " words ("
                << 100 * binary_file.tellg() / file_length << "%), ";
            status << "GOOD = " << data.GetNumChunks() << ", LOST = " << data.GetNumMissing();
            std::cout << "\r" << status.str();
        }
        

        if (full_spill) {
            if (debug_mode) {
                std::cout << std::endl << "full spill is true!" << std::endl;
                std::cout << "debug: Retrieved spill of " << nBytes << " bytes (" << nBytes / 4 << " words)\n";
                std::cout << "debug: Read up to word number " << binary_file.tellg() / 4 << " in input file\n";
            }
            if (!bad_spill) {
                if (debug_mode)
                    std::cout << "Spill is full and good!" << std::endl;
                unpacker_.ReadSpill(decodedList_, data_, nBytes / 4, is_verbose, debug_mode);   // Decoding data information from a good spill
                //IdleTask();
                if (debug_mode)
                    std::cout << std::endl << std::endl;
            }
            else {
                std::cout << " WARNING: Spill has been flagged as corrupt, skipping (at word " << binary_file.tellg() / 4
                        << " in file)!\n";
            }        

        }
        else if (debug_mode) {
            std::cout << std::endl << "Not full spill!" << std::endl;
            std::cout << "debug: Retrieved spill fragment of " << nBytes << " bytes (" << nBytes / 4 << " words)\n";
            std::cout << "debug: Read up to word number " << binary_file.tellg() / 4 << " in input file\n";
            std::cout << std::endl << std::endl;
        }
    
        num_spills_recvd++;
        pos_index = binary_file.tellg();
        
        if (debug_mode)
            std::cout << "Number of spills recorded (and parsed): " << num_spills_recvd << " spills" << std::endl;
        if (num_spills_recvd == max_num_spill && max_num_spill != 0) {                          // Reading until we reach the spill reading limit set in xia4ids.hh
            if (debug_mode)
                std::cout << "Limit of number of events to record = " << max_num_spill << " has been reached!" << std::endl;
            pos_index = binary_file.tellg();
            // pos_index -= ACTUAL_BUFF_SIZE*4;
            break;
        }
        
    } // Finished reading 'max_num_spill' spills

    delete[] data_;
    
	// Filter the Data Events and transfer the information into the DataArray 
    
    XiaData* decodedEvent;
    //ofstream myfile;
    //myfile.open("Parsing results.txt");
    
    for (int i = 0; i < decodedList_.size(); i++) {
        
        decodedEvent = decodedList_[i];
        
        //if (i != 0) {
            //myfile << "\n \n";
        //}
        //myfile << "Decoded event number " << i << ".\n";
        //myfile << "Event energy: " << decodedEvent->GetEnergy() << ".\n";
        //myfile << "Time stamp: " << decodedEvent->GetTime() << ".\n";
        //myfile << "Channel number: " << decodedEvent->GetChannelNumber() << ".\n";
        //myfile << "Module number: " << decodedEvent->GetModuleNumber() << ".\n";
        //myfile << "Pileup flag: " << decodedEvent->IsPileup() << ".\n";
        //myfile << "Out-of-range (saturated) flag: " << decodedEvent->IsSaturated() << ".\n";


		//Storing only data from ADCs defined in config
		if (tmc[decodedEvent->GetModuleNumber()][decodedEvent->GetChannelNumber()] == 0) continue;

		// Remove pileup and out-of-range events if the flags are set
		// stats[0=out-of-range, 1=pileup, 2=good][modnum][chnum]
		if (decodedEvent->IsSaturated()) {
			stats[0][decodedEvent->GetModuleNumber()][decodedEvent->GetChannelNumber()]++;
			if (reject_out) continue;
		}
		if (decodedEvent->IsPileup()) {
			stats[1][decodedEvent->GetModuleNumber()][decodedEvent->GetChannelNumber()]++;
			if (reject_pileup) continue;
		}
		
		// If we reach this stage, it means we have a good event, we store it
		stats[2][decodedEvent->GetModuleNumber()][decodedEvent->GetChannelNumber()]++;
        				
        // Transfer good signals to DataArray to build events.
        DataArray[i].chnum	= decodedEvent->GetChannelNumber();
        DataArray[i].modnum = decodedEvent->GetModuleNumber();
        DataArray[i].energy = calibrate(decodedEvent->GetModuleNumber(), decodedEvent->GetChannelNumber(), decodedEvent->GetEnergy());
        DataArray[i].time	= decodedEvent->GetTime() + delay[decodedEvent->GetModuleNumber()][decodedEvent->GetChannelNumber()];
        
        //Filling ROOT Histogram
		if (root == 1 && corr == 0) {
			int line = lmc[DataArray[i].modnum][DataArray[i].chnum];
			hStats->AddBinContent(line, 1);
			h[line]->Fill(DataArray[i].energy);
		}
		
		//In correlation mode, we need to delay the stop (stop = secondCh,secondMod)
		//The start is always the same reference and should not be used as stop
		if (corr > 0)
			for (j=0; j<corr; j++)
				if (DataArray[i].chnum == secondCh[j] && DataArray[i].modnum == secondMod[j])
					DataArray[i].time += CORR_DELAY*(int)corr_unit;
					
		//Cleaning up
		delete decodedList_[i]; 
				      

    }
    
        
    binary_file.close();
                
    return decodedList_.size();
}
