 /*  Reading the calibration file
  
  3rd degree polynomial:
  y = A + B*x + C*x^2 + D*x^3
  
  The format is:

  ADC ID = A B C D
  
  
  */


void read_cal(int argc, char **argv) {
  
  int status, id, count=0;
  double cal[MAX_CAL];
  FILE *cal_file;
  char line[100];
    
  if ( rate == 0 && (argc < 3 || !fopen(argv[2], "r")) ) {
    printf("No calibrations will be used: ...$n4i [config_file_name] [cal_file_name] \n");
    return;
  }
  else if ( rate == 1 && (argc < 4 || !fopen(argv[3], "r")) ) {
    printf("No calibrations will be used: ...$n4i [config_file_name] [input_file] [cal_file] \n");
    return;
  }
  else if (rate == 0 ) cal_file = fopen(argv[2], "r");
  else if (rate == 1 ) cal_file = fopen(argv[3], "r");
  
  
  while( fgets(line, 10000, cal_file) != NULL ) {
    
      // Parse the line
      status = sscanf(line, "%d=%lf%lf%lf%lf", &id, 
		     &cal[0], &cal[1], &cal[2], &cal[3]); //cal[4] is timing delay
      
      if (status != MAX_CAL+1 ) continue;
      if (id < 0 || id >= MAX_ID) continue;
      count++;

      // Store it
      for (i = 0; i < MAX_CAL; i++) calib[id][i] = cal[i];
   }
   
   printf("Read calibrations for %d channels.\n", count);
   
   have_cal = 1;
   
   
}



/*
 Calibration file example:
 
 # Clovers
 32=  0.229399 0.54564235 -3.358013E-8 0
 33=  0.09332  0.55244556 -4.380149E-8 0
 34= -0.016338 0.54922225 -1.267432E-7 0
 35= -0.025259 0.54017962 -4.713344E-8 0
 36=  0.089267 0.54183443 -6.125601E-8 0
 37=  0.068394 0.56837007 -5.948704E-8 0
 38=  0.062205 0.55030126 -2.787075E-8 0
 39=  0.390197 0.53972177 -6.842759E-8 0
 64=  0.216673 0.59845588 -6.950087E-8 0
 65=  0.062916 0.58516094 -5.351804E-8 0
 66=  0.012303 0.58202291  1.236904E-9 0
 67=  0.145814 0.57645731  3.037417E-9 0
 68= -0.083023 0.57738211 -6.365910E-8 0
 69=  0.02198  0.56058275 -1.175025E-8 0
 70=  0.362354 0.5384213  -5.546025E-8 0
 71=  0.150194 0.54730141 -7.776937E-8 0
 
 # LaBr5
 107=  3.845  0.736815  0.242337E-04  0.139650E-08
 # LaBr1
 108=  6.388  0.711998 -0.637028E-04  0.127655E-07

*/
