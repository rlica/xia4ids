
//Calibrating each signal, using random number generator
//If the signal has no calibration defined, the energy will be the same

int calibrate(int module, int channel, int energy) {
  
  
    if (have_cal == 0 || calib[module][channel][0] == 0) return energy;
	  
    int i;
    
    //srand(time(NULL)); this is very bad if doing it more than once per second, therefore we only do it once, in the main()
	
	double random_number, prob;
	double d_energy = 0;
	int    i_energy = 0;
	
	for (i=0; i<MAX_CAL; i++) d_energy += calib[module][channel][i] * pow(energy, i);
	 
	i_energy = d_energy;
	
    random_number =  (double)rand() / ((double)RAND_MAX + 1.0);
    
	prob = (d_energy - i_energy);

  	if (random_number < prob) i_energy += 1;
	
	return i_energy;

}



