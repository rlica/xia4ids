//Last edit: RL 22/05/2022

//RUN WITH: root -l -q macros/yield_hist.C"(Run_No)" 
//ROOT macro used for RILIS scans.

//Extracting areas from histograms



void yield_hist(const int Run_no){


int left, right, AREA=0, BG=0, run_log;
float RATE=0, rec_time=0;

TFile *f = new TFile(Form("Run%03d.root", Run_no));
TH1F *h1 = (TH1F*)f->Get("h16_Si");

FILE *outfile;
outfile=fopen(Form("out%d.txt", Run_no), "wt");
	
//Reading the recording time from log file
FILE *logfile;
logfile = fopen(Form("logs/log_Run%d.txt", Run_no), "r");
  if(!fscanf(logfile,"RUN #%d\n", &run_log)) { printf("ERROR: Cannot read logfile\n"); exit(0);}
  if(!fscanf(logfile,"Recorded in:  %f s\n",  &rec_time)) { printf("ERROR: Cannot read rectime from logfile\n"); exit(0);}

   
//--------------------------------------------------------------------------------------------- 
// Si gate
// no background

 left = 800;
right = 1100;

//Reading the AREA
AREA = h1->Integral(h1->FindBin(left), h1->FindBin(right));

cout << " Area = " << AREA << endl;

// Write to output file

fprintf(outfile,  "%d\t%.2f\t%d\t", Run_no, rec_time, AREA); // Add the next if muliple rates are enabled

//fprintf(outfile, "%.2f\t", RATE);

//---------------------------------------------------------------------------------------------





}
