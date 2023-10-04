/*
XIA DGF Pixie-16 .ldf to GASPware and ROOT converter

Authors:
R. Lica, IFIN-HH - CERN, razvan.lica@cern.ch 2020-2021 
K. Phan, TUNI - CERN Summer student 2020 (.ldf readout integration from PAAAS)

https://github.com/rlica/xia4ids

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <iomanip>
#include <random>

#include "TFile.h"
#include "TTree.h"
#include "TBrowser.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TROOT.h"

#include "xia4ids.hh"
#include "read_config.hh"
#include "calibrate.hh"
#include "read_cal.hh"
#include "merge_sort.hh"
#include "define_root.hh"
#include "event_builder.hh"
#include "event_builder_list.hh"
#include "event_builder_tree.hh"
#include "correlations.hh"
#include "write_correlations.hh"
#include "write_gasp.hh"
#include "write_stats.hh"
#include "write_list.hh"
#include "write_time.hh"
#include "read_ldf.hh"


int main(int argc, char *argv[]) {

    srand(time(0));
    
    printf("\n\t\t----------------------------------");
    printf("\n\t\t    XIA DGF Pixie-16 Converter");
    printf("\n\t\t https://github.com/rlica/xia4ids");
    printf("\n\t\t          Version = Jakku         ");
    printf("\n\t\t----------------------------------");
    printf("\n\n");

    read_config(argc, argv);

    read_cal(argc, argv);

    //Allocating memory
    DataArray = (struct dataStruct *)calloc(memoryuse, sizeof(struct dataStruct));
    TempArray = (struct dataStruct *)calloc(memoryuse, sizeof(struct dataStruct));

    if (gasp_format == 1 || list_format == 1) {
        EventArray = (struct Event *)calloc(memoryuse, sizeof(struct Event));
        GHeader    = (struct GaspRecHeader *)calloc(1, sizeof(struct GaspRecHeader));
    }

    // Reading run by run
    for (runnumber = runstart; runnumber <= runstop; runnumber++) {
		
        raw_list_size = 0, good_list_size = 0;
        totEvt = 0;
        tref = 0;
        run_good_chunks = 0;
        run_missing_chunks = 0;
        
        bool first_cycle = true; // to keep track of total run time

        // Open output file
        if (corr_format == 0) {

            
            outname = std::to_string(runnumber);
            if(runnumber < 1000) outname.insert(0, 3 - outname.size(), '0');
            outname = "Run" + outname;
            
            //GASPware format
            if (gasp_format == 1) {
                
                fp_out = fopen(outname.c_str(), "wt");
                if (!fp_out) {
                    fprintf(stderr, "ERROR: Unable to create %s - %m\n", outname.c_str());
                    return 0;
                }
            }

            //Event List format
            else if (list_format == 1) {
                
                outname = outname + ".list";
                fp_out = fopen(outname.c_str(), "wt");
                if (!fp_out) {
                    fprintf(stderr, "ERROR: Unable to create %s - %m\n", outname.c_str());
                    return 0;
                }
            }

            //ROOT format
            else if (root_format == 1 || stat_format == 1) {
                
                outname = outname + ".root";
                rootfile = new TFile(outname.c_str(), "recreate", "IDS data file");
                if (!rootfile) {
                    fprintf(stderr, "ERROR: Unable to create %s - %m\n", outname.c_str());
                    return 0;
                }
                define_root();
            }
        }    
        

			
		// Cycling over run partitions (a large run will be split into several files of 2.0 Gb each -> one run partition = one file)
		for (runpart = 0; runpart < 1000; runpart++) { 
           
			start_clock = (double)clock();
               
			//Ratemeter mode, analysing only the last RATE_EOF_MB megabytes from a file
			if (rate_format == 1) {
                
				if (argc < 3) { //Rate mode takes the input file as the second argument
					printf("Config file and input file required as arguments: ...$xia4ids config_file_name input file [calibration] \n");
                    return 0;
				}
                filename = argv[2];
				fp_in = fopen(argv[2], "rb");
				if (!fp_in) {
					printf("ERROR: Unable to open %s \n", filename.c_str());
                    return 0;
				}
				if (runpart > 0) break;
			}				
               
			// Normal mode analysing the full file
			if (rate_format == 0) {
                
                filename = std::to_string(runnumber);
                if(runnumber < 1000) filename.insert(0, 3 - filename.size(), '0');
                filename = runname + filename;
                if (runpart == 0) filename = filename + ".ldf";
                else filename = filename + "-" + std::to_string(runpart) + ".ldf";
                fp_in = fopen(filename.c_str(), "rb");
				if (!fp_in) {
					printf("File parsing completed: %s does not exist\n", filename.c_str());
					break;
				}
			}
			
			
			//Initializing the binary file object
			LDF_file ldf(filename.c_str());
			DATA_buffer data;
			int ldf_pos_index = 0;
			float progress = 0.0;

			// Set file length
			ldf.GetFile().open(ldf.GetName().c_str(), std::ios::binary);
			ldf.GetFile().seekg(0, ldf.GetFile().end);
			ldf.SetLength(ldf.GetFile().tellg());
			ldf.GetFile().seekg(0, ldf.GetFile().beg);
			ldf.GetFile().close();
			printf("Filename:  %s \nFile size: %.2f MB \n", filename.c_str(), float(ldf.GetFileLength())/1048576);


			// Start of a reading cycle:
			while (data.GetRetval() != 2 && ldf_pos_index <= ldf.GetFileLength()) {

				// iData will be the last data index.
				iData=0, iEvt=0;
                
				// Initializing the data array to zero
				memset(DataArray,0,memoryuse);  
				memset(TempArray,0,memoryuse); //Used only when sorting
                
				// Displaying the progress bar
				progress = float(ldf_pos_index) / float(ldf.GetFileLength());
				printProgress(progress);
                
				// read_ldf() will read a fixed number of spills at once from the binary file
				// if ratemode is enabled, read_ldf() will process only the last part of the file
				raw_list_size  += read_ldf(ldf, data, ldf_pos_index);
				good_list_size += iData;
				if (raw_list_size == 0)	continue;               					
                
				// Sorting the data chronologically.
				MergeSort(DataArray, TempArray, 0, iData);
                
				// Extract first and last time stamps 
				if (first_cycle) { 
					if (DataArray[1].time > 0)
						first_ts = DataArray[1].time;
					else
						printf("ERROR: Cannot read first timestamp \n");
					first_cycle = false;
				}
				if (DataArray[iData-1].time > 0)
					last_ts = DataArray[iData-1].time;
								
				// Writing statistics and skipping the event builder, will sort everything twice as fast
				if (stat_format == 1) continue;
				                       
				// Looking for correlations
				if (corr_format > 0)
					correlations();

				// Writing to GASPWare
				else if (gasp_format == 1) {
					event_builder();
					write_gasp();
					totEvt += iEvt;
					printf(" %3d events written to %s ", totEvt, outname.c_str());
					write_time(ldf_pos_index, ldf.GetFileLength());
				}

				// Writing event lists
				else if (list_format == 1) {
					event_builder_list();
					write_list();
					totEvt += iEvt;
					printf(" %3d events written to %s ", totEvt, outname.c_str());
					write_time(ldf_pos_index, ldf.GetFileLength());
				}

				// Writing to a ROOT Tree
				else if (root_format == 1 || stat_format == 1) {
					event_builder_tree();
					totEvt += iEvt;
					printf(" %3d events written to %s ", totEvt, outname.c_str());
					write_time(ldf_pos_index, ldf.GetFileLength());
				}

                                      
				// We break this loop after the entire file is read and parsed.
				if (data.GetRetval() == 2) { // last cycle.
					
					std::cout << std::endl;
					// std::cout << "First time stamp: " << first_ts << "\t Last time stamp: " << last_ts << std::endl;
					std::cout << "Finished reading complete file" << std::endl; 
                     
					// Checking file integrity 
					run_good_chunks += data.GetNumChunks(); 
					run_missing_chunks += data.GetNumMissing();
                                                     
                    break;
				} 
                                      
				// We also break this loop when reaching the initially read file length. 
				// This allows for reading out incomplete files or files that are currently being written.
				if (ldf_pos_index > ldf.GetFileLength()) {
					
                    std::cout << std::endl;
                    // std::cout << "First time stamp: " << first_ts << "\t Last time stamp: " << last_ts << std::endl;
                    std::cout << "Finished reading incomplete file" << std::endl; 
                    break;
				} 
	
				fflush(stdout);
				
			} // End of cycling over a partition
		
		} // End of cycling over all Run partitions

        // Printing statistics for each run if not in correlation mode
        // Writing the root file to disk
        if (corr_format == 0) {
            write_stats();
            memset(stats, 0, sizeof(stats));
            if (root_format == 1 || stat_format == 1) {
                rootfile->Write();
                rootfile->Save();
                rootfile->Close();
            }
        }

		// Writing correlation statistics for each run to file
		if (corr_format > 0)
			write_correlations();
        
        std::cout << "Sorting completed!" << std::endl;
    
    } // end of all Runs

    free(DataArray);
    free(TempArray);
    
    return 0;
} //end of main
