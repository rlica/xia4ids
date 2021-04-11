/*
Nutaq LyrTech to GASPware and ROOT converter
R. Lica, 2014 - 2016
razvan.lica@cern.ch

https://github.com/rlica/nutaq4ids

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

#include "nutaq4ids.hh"
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
#include "write_tree.hh"
#include "write_time.hh"
#include "read_ldf.hh"

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

void printProgress(double percentage) {
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    // fflush(stdout);
}

int main(int argc, char **argv)
{

    srand(time(0));

    printf("\n\t\t----------------------------------");
    printf("\n\t\t         Nutaq Converter");
    printf("\n\t\t           v10.02.2017  ");
    printf("\n\t\thttps://github.com/rlica/nutaq4ids");
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
        ofstream myfile,another;
        myfile.open("DataArray.txt");
        another.open("Before.txt");
        myfile  << "Time stamp"
                << "\t  " << "Energy"
                << "  " << "Module"
                << "  " << "Channel"
                << "\n\n";
                        another  << "Time stamp"
                << "\t  " << "Energy"
                << "  " << "Module"
                << "  " << "Channel"
                << "\n\n";
      
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

            // iData = 0, iEvt = 0;
            // read_grain();
            iData = read_ldf(tmc, ldf, data, ldf_pos_index);
            printf("Everything is ok, now ldf file is read!\n");

            // First and last time stamps for statistics.
            first_ts = DataArray[1].time;
            last_ts = DataArray[iData].time;

            if (root == 1)
            {
                event_builder_tree();
                //write_tree();
                totEvt += iEvt;
                // write_time();
            }
        }
        // Format: normal mode (not rate mode).
        else
            for (runpart = 0; runpart < 2; runpart++)
            {
                start_clock = (double)clock();
				if (runpart == 0)
					sprintf(filename, "%s%03d.ldf", runname, runnumber);
				else
					sprintf(filename, "%s%03d_%d.ldf", runname, runnumber, runpart);

                fp_in = fopen(filename, "rb");
                if (!fp_in)
                {
                    fprintf(stderr, "Unable to open %s - %m\n", filename);
                    break;
                }

                //Binary file object
                LDF_file ldf(filename);
                DATA_buffer data;
                int ldf_pos_index = 0;
                bool first_cycle = true;
                float progress = 0.0;
                int barWidth = 70;

                // Set file length then rewind to the beginning.
                binary_file.open(ldf.GetName().c_str(), std::ios::binary);
                binary_file.seekg(0, binary_file.end);
                ldf.SetLength(binary_file.tellg());
                binary_file.seekg(0, binary_file.beg);
                binary_file.close();

                // Start of a cycle:
                while (true) {
                    // Begin to parse ldf fielname.
                    // iData is now the last data index.
                    memset(DataArray,0,memoryuse + 10000);
                    memset(TempArray,0,memoryuse + 10000);

                    iData = read_ldf(tmc, ldf, data, ldf_pos_index);
        
                    progress = float(ldf_pos_index) / float(file_length);

                    printProgress(progress);

                    for (int k = 0; k < iData; k++) {
                        another  << std::setw(8) << std::fixed << DataArray[k].time
                                << std::setw(8) << DataArray[k].energy
                                << std::setw(8) << DataArray[k].modnum
                                << std::setw(8) << DataArray[k].chnum
                                << "\n";
                    } 
                    // Sorting the data chronologically.
                    MergeSort(DataArray, TempArray, 0, iData);

                    for (int k = 0; k < iData; k++) {
                        myfile  << std::setw(8) << std::fixed << DataArray[k].time
                                << std::setw(8) << DataArray[k].energy
                                << std::setw(8) << DataArray[k].modnum
                                << std::setw(8) << DataArray[k].chnum
                                << "\n";
                    }                    
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
                        printf(" %3d events written to %s ",
                            iEvt, outname);
                    }

                    printf("\t");

                    // Extract first and last time stamps for statistics.
                    if (first_cycle) { // first cycle.
                        first_ts = DataArray[1].time;
                        first_cycle = false;
                    }
                    if (data.GetRetval() == 2) { // last cycle.
                        last_ts = DataArray[iData].time;
                        std::cout << std::endl;
                        std::cout << "First time stamp: " << first_ts << std::endl;
                        std::cout << "Last time stamp: " << last_ts << std::endl; 
                                 
                        break; // We only break this loop after the entire file is read and parsed.
                    }

                    fflush(stdout);
                }
                //Printing statistics for each run if not in correlation mode
                if (corr == 0) {
                    write_stats();
                    memset(stats, 0, sizeof(stats));
                }                                   
            }
        myfile.close();
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
