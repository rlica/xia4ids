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

#include "TFile.h"
#include "TTree.h"
#include "TBrowser.h"
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


int main(int argc, char **argv)
{

    srand(time(0));

    printf("\n\t\t----------------------------------");
    printf("\n\t\t    XIA DGF Pixie-16 Converter");
    printf("\n\t\t           v04.2021  ");
    printf("\n\t\t https://github.com/rlica/xia4ids");
    printf("\n\t\t----------------------------------");
    printf("\n\n");

    read_config(argc, argv);

    // read_cal(argc, argv);

    //Allocating memory
    DataArray = (struct dataStruct *)calloc(memoryuse, sizeof(struct dataStruct));
    TempArray = (struct dataStruct *)calloc(memoryuse, sizeof(struct dataStruct));

    if (gasp == 1 || list == 1) {
		EventArray = (struct Event *)calloc(memoryuse, sizeof(struct Event));
		GHeader = (struct GaspRecHeader *)calloc(1, sizeof(struct GaspRecHeader));
    }

    // Reading run by run
    for (runnumber = runstart; runnumber <= runstop; runnumber++)
    {

        totEvt = 0;
        tref = 0;
        run_good_chunks = 0;
        run_missing_chunks = 0;
        
        bool first_cycle = true; // to keep track of total run time

        // Open output file
        if (corr == 0) {

            //GASPware format
            if (gasp == 1)
            {
                sprintf(outname, "Run%03d", runnumber);
                fp_out = fopen(outname, "wt");
                if (!fp_out)
                {
                    fprintf(stderr, "ERROR: Unable to create %s - %m\n", outname);
                    exit(0);
                }
            }

            //Event List format
            else if (list == 1)
            {
                sprintf(outname, "Run%03d.list", runnumber);
                fp_out = fopen(outname, "wt");
                if (!fp_out)
                {
                    fprintf(stderr, "ERROR: Unable to create %s - %m\n", outname);
                    exit(0);
                }
            }

            //ROOT format
            else if (root == 1) {
                sprintf(outname, "Run%03d.root", runnumber);
                rootfile = TFile::Open(outname, "recreate");
                if (!rootfile) {
                    fprintf(stderr, "ERROR: Unable to create %s - %m\n", outname);
                    exit(0);
                }
                define_root();
            }
        }    
        

			
				
        
        // Reading a run partition (a large run will be split into several files of 2.0 Gb each -> one run partition = one file)
            for (runpart = 0; runpart < 1000; runpart++) { 
            
                start_clock = (double)clock();
                
                //Running the code in rate mode, analysing only the last 10 MB from a file
                if (rate == 1) {   
					if (argc < 3) { //Rate mode takes the input file as the second argument
						printf("Config file and input file required as arguments: ...$xia4ids config_file_name input file [calibration] \n");
						exit(0);
					}
					
					sprintf(filename, "%s", argv[2]);
					fp_in = fopen(argv[2], "rb");
					if (!fp_in) {
						fprintf(stderr, "Unable to open %s - %m\n", filename);
						exit(0);
					}
				}
                
                // Normal mode (not ratemeter mode).
				else {
					if (runpart == 0)
						sprintf(filename, "%s%03d.ldf", runname, runnumber);
					else
						sprintf(filename, "%s%03d-%d.ldf", runname, runnumber, runpart);

					fp_in = fopen(filename, "rb");
					if (!fp_in) {
						// fprintf(stderr, "File does not exist %s - %m\n", filename);
						break;
					}
				}
				
				
                //Initializing the binary file object
                LDF_file ldf(filename);
                DATA_buffer data;
                int ldf_pos_index = 0;
                float progress = 0.0;

                // Set file length
                binary_file.open(ldf.GetName().c_str(), std::ios::binary);
                binary_file.seekg(0, binary_file.end);
                ldf.SetLength(binary_file.tellg());
                binary_file.seekg(0, binary_file.beg);
                binary_file.close();
                printf("Filename:  %s \nFile size: %.2f MB \n", filename, float(file_length)/1048576);
                
                if (rate == 1) 
					ldf_pos_index = int(file_length) - int (RATE_EOF_MB) * 1048576;


                // Start of a reading cycle:
                while (true) {

                    // iData will be the last data index.
                    iData=0, iEvt=0;
                    
                    //Initializing the data array to zero
                    memset(DataArray,0,memoryuse);  
                    memset(TempArray,0,memoryuse); //Used only when sorting 
                    
                    // read_ldf() will read a fixed number of spills at once from the binary file
                    iData = read_ldf(ldf, data, ldf_pos_index);
					
					// Displaying the progress bar
                    progress = float(ldf_pos_index) / float(file_length);
                    printProgress(progress);
                    
                    // Writing statistics and skipping the event builder
					if (stat == 1) {
						write_time(ldf_pos_index, file_length);
						continue;
					}
                                                           
                    // Sorting the data chronologically.
                    MergeSort(DataArray, TempArray, 0, iData);
                       
                    // Looking for correlations
                    if (corr > 0) 
						correlations();

                    // Writing to GASPWare
                    else if (gasp == 1) {
                      event_builder();
                      write_gasp();
                      totEvt += iEvt;
                      printf(" %3d events written to %s ", totEvt, outname);
                      write_time(ldf_pos_index, file_length);
                    }

                    // Writing event lists
                    else if (list == 1) {
                      event_builder_list();
                      write_list();
                      totEvt += iEvt;
                        printf(" %3d events written to %s ", totEvt, outname);
                        write_time(ldf_pos_index, file_length);
                    }

                    // Writing to a ROOT Tree
                    else if (root == 1) {
                        event_builder_tree();
                        totEvt += iEvt;
                        printf(" %3d events written to %s ", totEvt, outname);
                        write_time(ldf_pos_index, file_length);
                    }

                    
                    // Extract first and last time stamps for statistics.
                    if (first_cycle) { // first cycle.
                        first_ts = DataArray[1].time;
                        first_cycle = false;
                    }                    
                    if (DataArray[iData].time > 0)
						last_ts = DataArray[iData].time;
                              
                    
                    
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
                    if (ldf_pos_index > file_length) {
						
                        std::cout << std::endl;
                        // std::cout << "First time stamp: " << first_ts << "\t Last time stamp: " << last_ts << std::endl;
                        std::cout << "Finished reading incomplete file" << std::endl; 
                        break;
					} 
	
                    fflush(stdout);
                }
                                 
            } // End of Run partition

        // Printing statistics for each run if not in correlation mode
        // Writing the root file to disk
        if (corr == 0) {
            write_stats();
            memset(stats, 0, sizeof(stats));
			if (root == 1) {
				rootfile->Write();
				rootfile->Save();
				rootfile->Close();
			}
        }  

		// Writing correlation statistics for each run to file
		if (corr > 0)
			write_correlations();
        
        std::cout << "Sorting completed!" << std::endl;
    
    } // end of all Runs

    free(DataArray);
    free(TempArray);
    exit(0);

} //end of main
