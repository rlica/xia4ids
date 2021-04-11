//   Reading parameters from config file

#include <iostream>


void read_config(int argc, char **argv) {
  
    
  if (argc < 2) {
      printf("Config file required as argument: ...$n4i [config_file_name] \n");
    exit(0);
  }

  FILE *input_file;
  input_file = fopen( argv[1], "r");
  
  if(!fscanf(input_file,"runName   %s\n",      runname)) { printf("ERROR: Cannot read <runName> from '%s'\n",  argv[1]); exit(0);}
  if(!fscanf(input_file,"timegate  %d\n",    &timegate)) { printf("ERROR: Cannot read <timegate> from '%s'\n", argv[1]); exit(0);}
  if(!fscanf(input_file,"cs-tac    %d\n",      &cs_tac)) { printf("ERROR: Cannot read <cs_tac> from '%s'\n",   argv[1]); exit(0);}
  if(!fscanf(input_file,"pair-tac  %d\n",    &pair_tac)) { printf("ERROR: Cannot read <pair_tac> from '%s'\n", argv[1]); exit(0);}
  if(!fscanf(input_file,"reftype   %d\n",     &reftype)) { printf("ERROR: Cannot read <reftype> from '%s'\n",  argv[1]); exit(0);}
  if(!fscanf(input_file,"flagtype  %d\n",    &flagtype)) { printf("ERROR: Cannot read <flagtype> from '%s'\n", argv[1]); exit(0);}


  //Reading ref_unit
  if(!fscanf(input_file,"ref_unit   %llu %s\n",  &ref_unit, ref_string)) { 
    printf("ERROR: Cannot read <ref_unit> from '%s'. Format [int] [string] where [string]={ns, us, ms, s}\n",  argv[1]); 
    exit(0);
  }
  
      //checking the unit time. strncmp returns 0 if the strings are identical (very wierd)
  if      ( strncmp(ref_string, "us", 2) == 0 )  ref_unit = ref_unit*US;
  else if ( strncmp(ref_string, "ms", 2) == 0 )  ref_unit = ref_unit*MS;
  else if ( strncmp(ref_string,  "s", 1) == 0 )  ref_unit = ref_unit*S;
  else if ( strncmp(ref_string, "ns", 2) == 0 )  {
    ref_unit = ref_unit/10;
    if (ref_unit < 1) {
      printf("ERROR: <ref_unit> from '%s' cannot be less than '10 ns'.\n", argv[1]);
      exit(0);
    }
  }
  else {
    printf("ERROR: Cannot read <ref_unit> from '%s'. Format [int] [string] where [string]={ns, us, ms, s}\n",  argv[1]); 
    exit(0);
  }




  //Reading run_unit - seconds, minutes or hours
  if(!fscanf(input_file,"run_unit   %llu %s\n",  &run_unit, run_string)) { 
    printf("ERROR: Cannot read <ref_unit> from '%s'. Format [int] [string] where [string]={s, m, h}\n",  argv[1]); 
    exit(0);
  }
  
  if      ( strncmp(run_string, "h", 2) == 0 )   run_unit = run_unit*S*3600;
  else if ( strncmp(run_string, "m", 2) == 0 )   run_unit = run_unit*S*60;
  else if ( strncmp(run_string, "ms", 2) == 0 )  run_unit = run_unit*MS;
  else if ( strncmp(run_string, "us", 2) == 0 )  run_unit = run_unit*US;
  else if ( strncmp(run_string,  "s", 1) == 0 )  run_unit = run_unit*S;
  else if ( strncmp(run_string, "ns", 2) == 0 )  {
    run_unit = run_unit/10;
    if (run_unit < 1) {
      printf("ERROR: <ref_unit> from '%s' cannot be less than '10 ns'.\n", argv[1]);
      exit(0);
    }
  }
  else {
    printf("ERROR: Cannot read <run_unit> from '%s'. Format [int] [string] where [string]={ns, us, ms, s, m, h}\n",  argv[1]); 
    exit(0);
  } 
  
    
  
  //checking the format
  if(!fscanf(input_file,"Format    %s\n",   format_string)) {
    printf("ERROR: Cannot read <Format> from '%s'. Format {gasp, list, stats, root}\n",  argv[1]);
    exit(0);
  }  
  if      ( strncmp(format_string, "stat", 4) == 0 )  stat = 1;
  else if ( strncmp(format_string, "rate", 4) == 0 )  rate = 1;
  else if ( strncmp(format_string, "list", 4) == 0 )  list = 1;
  else if ( strncmp(format_string, "gasp", 4) == 0 )  gasp = 1;
  else if ( strncmp(format_string, "root", 4) == 0 )  root = 1;
  else {
    printf("ERROR: Cannot read <Format> from '%s'. Format {gasp, list, stats, root}\n",  argv[1]);
    exit(0);
  } 
  
  
  
  //Coincidence level (fold)
  if(!fscanf(input_file,"Fold  %d\n",    &fold)) { printf("ERROR: Cannot read <fold> from '%s'\n", argv[1]); exit(0);}
  
  
  printf("Successfully read configuration parameters. \n");


  
  
  
  ///////////////////////////////////////////////////////////////////////////////////////////////
  //Reading the Configuration coding
  fscanf(input_file,"|Index\t|Type\t|Mod\t|Chan\t|Delay\t|Link\t|Start\t|Stop\t|Comment\n");
  //     0       1      2     3      4       5      6       7      string
  
  int line=0, column=0;
  while ( fscanf(input_file,"%d", &config_coding[line][column]) ) {
    //printf("%d\t", config_coding[line][column]);
            
    if (column<7) column++;
    else {
      
      getline(&comment_line, &nbytes, input_file);
      sscanf(comment_line, "%s", comment_string[line]);
      column=0;
      line++;
      //printf("\n");
    }    
  }
  
  
  //correlation mode will skip the event builder
  if( fscanf(input_file,"Correlations\n%d %d %d %d\n", &firstMod[corr],
                                            &firstCh[corr],
	 	                          &secondMod[corr],
		                           &secondCh[corr] ) == 4 ) {
    corr++;
    while ( fscanf(input_file,"%d %d %d %d\n", &firstMod[corr],
                                       &firstCh[corr],
		                     &secondMod[corr],
		                      &secondCh[corr] ) == 4 ) corr++;
    
  }
  
  
  
  
  //finding how many lines were successfully read
  detnum = line;
  if (line == 0 || column != 0) { 
    printf("ERROR: Cannot read configuration coding from '%s'. \n", argv[1]); 
    exit(0);
  }
  
  
  //ratemode needs 2 arguments
  if (rate == 1 && argc < 3) {
      printf("Input file (raw data run) required as second argument: ...$n4i [config_file] [input_file] \n");
    exit(0);
  }
  
  
  for (i=0; i<detnum; i++) {
    
    // finding maximum number of detectors of the same type
    if (config_coding[i][0] > maxnum[config_coding[i][1]])
      maxnum[config_coding[i][1]] = config_coding[i][0]; 
    
    //finding how many detector types and modules exist
    if (config_coding[i][1] > dettypes) dettypes = config_coding[i][1]; 
    if (config_coding[i][2] > modules)   modules = config_coding[i][2];
    
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // error messages
    
    if (runstart < 0 || runstop < 0 || runstart > runstop) {
      printf("ERROR: Invalid run numbers.\n");
      printf("       ----> Run Start = %d     Run Stop = %d\n", runstart, runstop);
      exit(0);
    }
    
    if (config_coding[i][0] > MAX_NUM_DET || config_coding[i][0] < 1 ) {
      printf("ERROR: In configuration file '%s': Index must be between 1 and %d.\n", argv[1], MAX_NUM_DET);
      printf("       ----> Coding Line %d: Index = %d\n", i, config_coding[i][0]);
      exit(0);
    }
    
    if (config_coding[i][1] > MAX_NUM_DETTYPES || config_coding[i][1] < 1 ) {
      printf("ERROR: In configuration file '%s': Detector type must be between 1 and %d.\n", argv[1], MAX_NUM_DETTYPES);
      printf("       ----> Coding Line %d: Dettype = %d\n", i, config_coding[i][1]);
      exit(0);
    }
    
    if (i==0 && config_coding[i][1] != 1 ) {
      printf("ERROR: In configuration file '%s': Detector type must start from 1.\n", argv[1]);
      printf("       ----> Coding Line %d: Dettype = %d\n", i, config_coding[i][1]);
      exit(0);
    }
    
    if ( i>0 &&
         ( (config_coding[i-1][1] > config_coding[i][1]) || 
           (config_coding[i][1] - config_coding[i-1][1]) > 1 )   ) {
      printf("ERROR: In configuration file '%s': Type numbering must be incremental.\n", argv[1]);
      printf("       ----> Coding Line %d: Type = %d\n", i, config_coding[i][1]);
      exit(0);
    }
    
    // if (config_coding[i][2] > MAX_NUM_MOD || config_coding[i][2] < 1 ) {
    //   printf("ERROR: In configuration file '%s': Module number must be between 1 and %d.\n", argv[1], MAX_NUM_DETTYPES);
    //   printf("       ----> Coding Line %d: MOD = %d\n", i, config_coding[i][2]);
    //   exit(0);
    // }
    
    //if (config_coding[i][3] > MAX_NUM_CHN || config_coding[i][3] < 1 ) {
      //printf("ERROR: In configuration file '%s': Channel number must be between 1 and %d.\n", argv[1], MAX_NUM_CHN);
      //printf("       ----> Coding Line %d: CHAN = %d\n", i, config_coding[i][3]);
      //exit(0);
    //}
    
    if (i>0 && 
        config_coding[i-1][1] == config_coding[i][1] && 
        config_coding[i-1][5] != config_coding[i][5]) {
      printf("ERROR: In configuration file '%s': Link must be the same for all the detectors of the same type.\n", argv[1]);
      printf("       ----> Coding Line %d: Link = %d\n", i, config_coding[i][5]);
      exit(0);
    }
    
    if ( (config_coding[i][1] == cs_tac || config_coding[i][1] == pair_tac)  && 
         (config_coding[i][5] < 1       || config_coding[i][5] >= config_coding[i][1]) ) {
      printf("ERROR: In configuration file '%s': Link must be between 1 and %d for TAC.\n", argv[1], config_coding[i][1]);
      printf("       ----> Coding Line %d: Link = %d\n", i, config_coding[i][5]);
      exit(0);
    }
  
    
    if (config_coding[i][6] == config_coding[i][7] && config_coding[i][1] == pair_tac) {
      printf("ERROR: In configuration file '%s': Start cannot be identical to stop.\n", argv[1]);
      printf("       ----> Coding Line %d: Start = %d, Stop = %d\n", i, config_coding[i][6], config_coding[i][7]);
      exit(0);
    }
    
    
    
    
    
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    //Storing the configuration coding


      //11.09.2015 - To be able to define same signal to multiple detectors (eg. veto)
      //if the same mod+ch+type is defined as multiple indexes,
	    //the index will be 10203 (if defined as 1,2,3) or 141516(if 14,15,16)

    if (ntmc[config_coding[i][1]][config_coding[i][2]][config_coding[i][3]] == 0) {
      ntmc[config_coding[i][1]][config_coding[i][2]][config_coding[i][3]]=config_coding[i][0];
    }
    else {
       ntmc[config_coding[i][1]][config_coding[i][2]][config_coding[i][3]] = 100*ntmc[config_coding[i][1]][config_coding[i][2]][config_coding[i][3]] + config_coding[i][0];
     }

     
    
        tmc[config_coding[i][2]][config_coding[i][3]]=config_coding[i][1];
			  lmc[config_coding[i][2]][config_coding[i][3]]=i;
			delay[config_coding[i][2]][config_coding[i][3]]=config_coding[i][4];
  link_type[config_coding[i][1]]                     =config_coding[i][5];
			start[config_coding[i][0]][config_coding[i][1]]=config_coding[i][6];
			 stop[config_coding[i][0]][config_coding[i][1]]=config_coding[i][7];
   

	
    
  }
  
  if (dettypes == 0 || modules == 0 || dettypes > 30 || modules > 10) {
    printf("ERROR: In configuration file '%s'. Dettypes=%d, Modules=%d\n", argv[1], dettypes, modules);
    exit(0);
  }
  
  else {
    printf ("  - %d configuration coding lines\n", detnum);
    printf ("  - %d detector types\n",             dettypes);
  }
  

  
  
  //~ if (corr > 0) {
      //~ printf("Looking for %d timing correlations. Will skip the event builder:\n", corr);
      //~ return; 
    //~ }

  
  
  
  //LIST MODE: finding the reference detector for timestamp difference
  //Here we define the size of the event, not including the zero separator
  list_evSize = dettypes;
  
  for (i=1; i<=dettypes; i++)
    if (link_type[i]>0) {
      list_evSize++;
      if (reference_link>0 && reference_link!=link_type[i]) {
        printf("ERROR: Only one reference link is currently allowed in list mode. Check the configuration file.\n");
        exit(0);
      }
      
      else reference_link = link_type[i];
    }
  
  
  if (rate == 0) {                       //Normal mode
  printf("Start with run number:\t");
  // scanf("%d", &runstart);
  std::cin >> runstart;
  printf("Stop at run number:\t");
  // scanf("%d", &runstop);
  std::cin >> runstop;
  }
  
  else {                                //Ratemeter mode
  // printf("How many blocks (buffers) from the end of file you want to read? \t");
  // scanf("%d", &rate_eof_blocks);
  //rate_eof_blocks = 10;
  printf("Write data in a ROOT file? (y/n) \t");
  scanf("%s", rate_root_string);
  if ( strncmp(rate_root_string, "y", 1) == 0 )      root = 1; 
  else if ( strncmp(rate_root_string, "n", 1) == 0 ) root = 0;
  else  {
        printf("ERROR: Bad input\n");
        exit(0);
      }   
    
  }
  
  fclose(input_file);


}


/*
-----------------------------------------------------

1) Numbering starts from 1

2) If you don't want correlations, just delete them

-------------------------------------------------------
*/



