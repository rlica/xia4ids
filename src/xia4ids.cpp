/*
XIA DGF Pixie-16 .ldf to GASPware and ROOT converter
R. Lica, 2021
razvan.lica@cern.ch

https://github.com/rlica/xia4ids
* 
* First version: Khai Phan, TUNI - CERN Summer student 2020.

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
    printf("\n\t\t           v11.04.2021  ");
    printf("\n\t\t https://github.com/rlica/xia4ids");
    printf("\n\t\t----------------------------------");
    printf("\n\n");

    read_config(argc, argv);

    // read_cal(argc, argv);

    //Allocating memory
    DataArray = (struct data *)calloc(memoryuse + 10000, sizeof(struct data));
    TempArray = (struct data *)calloc(memoryuse + 10000, sizeof(struct data));


    //if (root == 1)
        //TEventArray = (struct TEvent *)calloc(memoryuse + 10000, sizeof(struct TEvent));

    if (gasp == 1 || list == 1)
    {
        EventArray = (struct Event *)calloc(memoryuse + 10000, sizeof(struct Event));
        GHeader = (struct GaspRecHeader *)calloc(1, sizeof(struct GaspRecHeader));
    }

    /////////////////////////////////////////////////////////////////// Reading run by run
    for (runnumber = runstart; runnumber <= runstop; runnumber++)
    {

        totEvt = 0;
        tref = 0;
        bool first_cycle = true; // to keep track of total run time

        // Open output file
        if ((gasp == 1 || list == 1 || root == 1) && corr == 0)
        {

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
        
      
        if (rate == 1)
        { //Rate mode takes the input file as the second argument
            if (argc < 3)
            {
                printf("Config file and input file required as arguments: ...$n4i [config_file_name] [input file] \n");
                exit(0);
            }
            sprintf(filename, "%s", argv[2]);
            fp_in = fopen(argv[2], "rb");
            if (!fp_in)
            {
                fprintf(stderr, "Unable to open %s - %m\n", filename);
                exit(0);
            }

            //Binary file object
            LDF_file ldf(filename);
            DATA_buffer data;
            int ldf_pos_index = 0;


            start_clock = (double)clock();

            printf("Reading .ldf file %s \n", filename);
            iData = read_ldf(tmc, ldf, data, ldf_pos_index);

            // First and last time stamps for statistics.
            first_ts = DataArray[1].time;
            last_ts = DataArray[iData].time;

            if (root == 1)
            {
                event_builder_tree();
                totEvt += iEvt;
            }
        }
        // Format: normal mode (not rate mode).
        else
            for (runpart = 0; runpart < 1000; runpart++) // Run partition (one run partition = one file, a large run will be split into several files of 2.0 Gb each)
            {
                start_clock = (double)clock();
				if (runpart == 0)
					sprintf(filename, "%s%03d.ldf", runname, runnumber);
				else
					sprintf(filename, "%s%03d-%d.ldf", runname, runnumber, runpart);

                fp_in = fopen(filename, "rb");
                if (!fp_in)
                {
                    // fprintf(stderr, "File does not exist %s - %m\n", filename);
                    break;
                }

                //Binary file object
                LDF_file ldf(filename);
                DATA_buffer data;
                int ldf_pos_index = 0;
                float progress = 0.0;


                // Set file length then rewind to the beginning.
                binary_file.open(ldf.GetName().c_str(), std::ios::binary);
                binary_file.seekg(0, binary_file.end);
                ldf.SetLength(binary_file.tellg());
                binary_file.seekg(0, binary_file.beg);
                binary_file.close();
                printf("Filename:  %s \nFile size: %.2f MB \n", filename, float(file_length)/1048576);

                // Start of a reading cycle:
                while (true) {
                    // Begin to parse ldf filename.
                    // iData is now the last data index.
                    memset(DataArray,0,memoryuse + 10000);  //Initializing the data array to zero
                    memset(TempArray,0,memoryuse + 10000);
                    
                    // read_ldf() will read a fixed number of spills at once from the binary file
                    iData = read_ldf(tmc, ldf, data, ldf_pos_index);
        
                    progress = float(ldf_pos_index) / float(file_length);

                    printProgress(progress);

                   
                    // Sorting the data chronologically.
                    MergeSort(DataArray, TempArray, 0, iData);

                       
                    ////Looking for correlations
                    //if (corr > 0)
                    //{
                        //correlations();
                        //continue;
                    //}

                    //// Writing to GASPWare
                    //else if (gasp == 1)
                    //{
                        //event_builder();
                        //write_gasp();
                        //totEvt += iEvt;

                        //printf(" (%3d ev/blk) %9d events written to %s ",
                            //iEvt / (current_block - prev_block + 1), totEvt, outname); //Fixed a very old bug -> add 1 to avoid division by zero (AAAAAAARGH)

                        //// write_time();
                    //}

                    //// Writing event lists
                    //else if (list == 1)
                    //{
                        //event_builder_list();
                        //write_list();
                        //totEvt += iEvt;
                        //printf(" (%3d ev/blk) %9d events written to %s ",
                            //iEvt / (current_block - prev_block + 1), totEvt, outname);
                        //// write_time();
                    //}

                    // Writing a ROOT Tree
                    if (root == 1)
                    {
                        event_builder_tree();
                        totEvt += iEvt;
                        printf(" %3d events written to %s ", iEvt, outname);
                        write_time(ldf_pos_index, file_length);
                    }
                    printf("\t");


                    // Extract first and last time stamps for statistics.
                    if (first_cycle) { // first cycle.
                        first_ts = DataArray[1].time;
                        first_cycle = false;
                    }
                    if (DataArray[iData].time > 0) last_ts = DataArray[iData].time;
                    
                    // We break this loop after the entire file is read and parsed.
                    if (data.GetRetval() == 2) { // last cycle.

                       std::cout << std::endl;
                       // std::cout << "First time stamp: " << first_ts << "\t Last time stamp: " << last_ts << std::endl;
                       std::cout << "Finished reading complete file" << std::endl; 
                                
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
                                 
            } ////// End of Run partition (one run partition = one file, a large run will be split into several files of 2.0 Gb each)

        //Printing statistics for each run if not in correlation mode
        if (corr == 0) {
            write_stats();
            memset(stats, 0, sizeof(stats));
        }  
        
        if (root == 1 && corr == 0)
        {
            // Write everything
            rootfile->Write();
            // Close output root file
            rootfile->Save();
            rootfile->Close();
        }

        std::cout << "Finish run!" << std::endl;
    } ////////////////////////////////////////////////////////////// end of Run

    //Writing correlation statistics for all runs to file
    if (corr > 0)
        write_correlations();

    free(DataArray);
    free(TempArray);
    exit(0);

} //end of main
