
#define MAX_NUM_DET      300
#define MAX_NUM_DETTYPES  10
#define MAX_NUM_CHN       17
#define MAX_NUM_MOD       15
#define MAX_CAL            4
#define memoryuse    4000000 // Size of the data array
#define max_num_spill     20 // Limit of number of spills to read into the data array.
#define reject_pileup      1 // Reject events marked as pileup (set to 0 to treat them as good data)
#define reject_out		   1 // Reject events marked as out-of-range (set to 0 to treat them as good data)


////if digitizer unit time = 10ns 
//#define US 100
//#define MS 100000
//#define  S 100000000

//XIA DGF Pixie-16 250MHz - digitizer unit time = 4ns
#define US 250
#define MS 250000
#define  S 250000000

//correlation prompt delay and histogram range
#define CORR_DELAY 4000
#define RANGE      8000

//progress bar specs
#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||"  
#define PBWIDTH 50

typedef struct data { 
  uint64_t  time;
  short int energy;
  short int chnum;
  short int modnum;
} data;

typedef struct Event {
  short int elem[MAX_NUM_DET];
  short int evSize;
} Event;


typedef struct GaspRecHeader {
  short int head[16];
} GaspRecHeader;



//  GLOBAL VARIABLES
//everything is initialized 0 by default

  float tot_run_time=0; //in seconds
  int   tot_sig_read;   //total number of signals read
 
  FILE *fp_in; 
  FILE *fp_out;
  char filename[300];
  char outname[100];
  char runname[200];
  char ref_string[2], run_string[2], format_string[10], rate_root_string[1];
  unsigned long long int ref_unit, run_unit, corr_unit=1;
  int i, j, k, l, m, n, e, z,
      iData = 0, iEvt, totEvt,
      runstart, runnumber, runstop, runpart,
      timegate, 
      detnum,
      maxnum[MAX_NUM_DETTYPES],
      dettypes,
      modules,
      cs_tac, pair_tac, 
      reftype, 
      flagtype, flag,
      fold;
      // rate_eof_blocks;
  uint32_t blocklen;
  char check_block_size,
       end_of_file;
  uint64_t tref, next_tref;     //tref is global, it starts initialized as zero,
                                //we change it only when encountering a new reference.
                                //never reset it to zero !!! NO PROTONS LEFT BEHIND!!!
  uint64_t first_ts = 0, last_ts = 0;
  int config_coding[200][8]; //config_coding[lines][columns]
                                             
  
  // ADC identifiers
  //         tmc[modnum][chnum] = det type 
  //  ntmc[type][modnum][chnum] = det number                         

  int  tmc                  [MAX_NUM_MOD][MAX_NUM_CHN],   // detector type identifier w.r.t. the module and channel number          
      ntmc[MAX_NUM_DETTYPES][MAX_NUM_MOD][MAX_NUM_CHN],   // detector number identifier 
     delay[MAX_NUM_MOD][MAX_NUM_CHN],        // timestamp delay to have the signals in sync
      link_type[MAX_NUM_DETTYPES],           // link_type[type_to_be_linked]
     start[MAX_NUM_DET][MAX_NUM_DETTYPES],   // start for the pair_tac
     stop[MAX_NUM_DET][MAX_NUM_DETTYPES],    // stop for the pair_tac   
       lmc[MAX_NUM_MOD][MAX_NUM_CHN];        // line [mod][chan] -> returns the index of the config line
  
  int corr, list, gasp, root, stat, rate, rate_root;
  int list_evSize, reference_link;
  int corr_hist[MAX_NUM_DET][RANGE];
  int firstMod[MAX_NUM_DET], secondMod[MAX_NUM_DET], firstCh[MAX_NUM_DET], secondCh[MAX_NUM_DET];
  
  long long int multiplier=1; //low resolution time multiplier
  //multiplier=1000;  // -> 1000*1ns = 1 microsecond  
  
  int stats[3][MAX_NUM_MOD][MAX_NUM_CHN]; // stats[0=out-of-range, 1=pileup, 2=good][modnum][chnum]
  
  int have_cal;
  double calib[MAX_NUM_MOD][MAX_NUM_CHN][MAX_CAL];
 
  short int zero[10000];
  double start_clock;
  size_t nbytes=100;
  
  
  struct           Event *EventArray;
  struct   GaspRecHeader *GHeader; 
  struct            data *DataArray; 
  struct            data *TempArray; 


// ROOT Variables and objects
TFile *rootfile;
TTree *tree;
TH1F *hStats, *h[MAX_NUM_DET];

int   E_branch[MAX_NUM_DETTYPES][MAX_NUM_DET],
      T_branch[MAX_NUM_DETTYPES][MAX_NUM_DET],
      M_branch[MAX_NUM_DETTYPES],
      MULT_branch;
ULong64_t  TIME_REF_branch;
ULong64_t  TIME_RUN_branch;    
char root_string[MAX_NUM_DET][100];
   
char *comment_line;
char comment_string[MAX_NUM_DET][100];

