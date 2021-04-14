
#define MAX_NUM_DET       70
#define MAX_NUM_DETTYPES  30
#define MAX_NUM_CHN       17
#define MAX_NUM_MOD       10
#define MAX_CAL            4
#define MAX_ID        0x1000
#define memoryuse    4000000


////if digitizer unit time = 10ns 
//#define US 100
//#define MS 100000
//#define  S 100000000

//if digitizer unit time = 4ns
#define US 250
#define MS 250000
#define  S 250000000

//correlation prompt delay and histogram range
#define CORR_DELAY 4000
#define RANGE      8000



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

//typedef struct TEvent {
  //int E[MAX_NUM_DETTYPES][MAX_NUM_DET];
  //int T[MAX_NUM_DETTYPES][MAX_NUM_DET];
  //int M[MAX_NUM_DETTYPES];
  //int MULT;
  //ULong64_t TIME_REF;
  //ULong64_t TIME_RUN;
//} TEvent;


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
  int full_size, current_block, prev_block; //in blocks
  char check_block_size,
       end_of_file;
  uint64_t tref, next_tref;     //tref is global, it starts initialized as zero,
                                //we change it only when encountering a new reference.
                                //never reset it to zero !!! NO PROTONS LEFT BEHIND!!!
  uint64_t first_ts = 0, last_ts = 0;
  int config_coding[200][8]; //config_coding[lines][columns]
                                             
  
  //                    ADC identifiers
  //           tmc[modnum][chnum] = [type] 
  //  ntmc[type][modnum][chnum] = [number]                         
  
  int  tmc                  [MAX_NUM_MOD][MAX_NUM_CHN],           
      ntmc[MAX_NUM_DETTYPES][MAX_NUM_MOD][MAX_NUM_CHN],  
     delay[MAX_NUM_MOD][MAX_NUM_CHN],                            // delay                     [modules][channels]
      link_type[MAX_NUM_DETTYPES],                  // link_type[type_to_be_linked]
     start[MAX_NUM_DET][MAX_NUM_DETTYPES],   //start and stop for the pair_tac
     stop[MAX_NUM_DET][MAX_NUM_DETTYPES],    //   
       lmc[MAX_NUM_MOD][MAX_NUM_CHN];       // line [mod][chan] -> returns the config line
  
  int corr, list, gasp, root, stat, rate, rate_root;
  int list_evSize, reference_link;
  int corr_hist[MAX_NUM_DET][RANGE];
  int firstMod[MAX_NUM_DET], secondMod[MAX_NUM_DET], firstCh[MAX_NUM_DET], secondCh[MAX_NUM_DET];
  
  long long int multiplier=1; //low resolution time multiplier
  //multiplier=1000;  // -> 1000*1ns = 1 microsecond  
  
  int stats[3][MAX_NUM_MOD][MAX_NUM_CHN]; // stats[0=fail, 1=pileup, 2=total][modnum][chnum]
  
  int have_cal;
  double calib[MAX_ID][MAX_CAL];
 
  short int zero[10000];
  
  
  struct           Event *EventArray;
  //struct          TEvent *TEventArray;
  struct   GaspRecHeader *GHeader; 
  struct            data *DataArray; 
  struct            data *TempArray; 



TFile *rootfile;
TTree *tree;
TH1F *hStats, *h[MAX_NUM_DETTYPES];

int   E_branch[MAX_NUM_DETTYPES][MAX_NUM_DET],
      T_branch[MAX_NUM_DETTYPES][MAX_NUM_DET],
      M_branch[MAX_NUM_DETTYPES],
      MULT_branch;
ULong64_t  TIME_REF_branch;
ULong64_t  TIME_RUN_branch;    
char root_string[MAX_NUM_DETTYPES][100];

    
double start_clock;
size_t nbytes=100;
char *comment_line;
char comment_string[MAX_NUM_DETTYPES][100];

