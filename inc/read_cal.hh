 /*  Reading the calibration file
  
  3rd degree polynomial:
  y = A + B*x + C*x^2 + D*x^3
  
  The calibration file format is:

  MODULE1 CHANNEL1 A1 B1 C1 D1
  MODULE2 CHANNEL2 A2 B2 C2 D2
  * 
  * 
  * 
  
  */


void read_cal(int argc, char **argv) {
  
  int status, module, channel, count=0;
  double cal[MAX_CAL];
  FILE *cal_file;
  char line[10000];
    
  if ( rate == 0 && (argc < 3 || !fopen(argv[2], "r")) ) {
    printf("No calibrations will be used: ...$xia4ids [config_file_name] [cal_file_name] \n");
    return;
  }
  else if ( rate == 1 && (argc < 4 || !fopen(argv[3], "r")) ) {
    printf("No calibrations will be used: ...$xia4ids [config_file_name] [input_file] [cal_file] \n");
    return;
  }
  else if (rate == 0 ) cal_file = fopen(argv[2], "r");
  else if (rate == 1 ) cal_file = fopen(argv[3], "r");
  
  
  while( fgets(line, 10000, cal_file) != NULL ) {
    
      // Parse the line
      status = sscanf(line, "%d%d%lf%lf%lf%lf", &module, &channel, &cal[0], &cal[1], &cal[2], &cal[3]);
      
      if (status != MAX_CAL+1 ) {
		  printf("Cal file read error: module %d channel %d \n", module, channel);
		  continue;
	  }
      if (module < 0 || module >= MAX_NUM_MOD) {
		  printf("Cal file read error: module %d \n", module);
		  continue;
	  }
      if (channel < 0 || channel >= MAX_NUM_CHN) {
		  printf("Cal file read error: channel %d \n", channel);
		  continue;
	  }

      count++;

      // Store it
      for (i = 0; i < MAX_CAL; i++) calib[module][channel][i] = cal[i];
   }
   
   printf("Successfull reading of calibration coefficients for %d channels.\n", count);
   
   have_cal = 1;
   
   
}

