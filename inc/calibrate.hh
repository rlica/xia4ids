//Calibrating each signal, using random number generator
//If the signal has no calibration defined, the energy will be the same

double calibrate(int module, int channel, int energy) {
  
    // If ADC energy is 0, return 0
    if (energy < 1) return 0.;
    // If no calibration for channel, return ADC energy
    if (!have_cal || calib[module][channel][0] == 0) return energy;
    // If the user wants to disable a channel (slope = 999.), return energy = 0
    if (have_cal && calib[module][channel][1] == 999) return 0.; // Best way to remove det from data that are defined on the config file. (To avoid Jaime's and Marcos' effect)
	  
    // Generate random number between 0 and 1, and add to ADC channel
    // to remove binning issues
//    std::random_devide rmdseed;
//    std::default_random_engine generator(rmdseed());
//    std::uniform_real_distribution<double> distribution(-0.5,0.5);
//    double engPlusRand = energy + distribution(generator);
    double engPlusRand = energy + (RandValue->Rndm() - 0.5);
    
    // Apply calibration and return energy
    double d_energy = 0.;
    for(int i=0; i<MAX_CAL; i++) d_energy += calib[module][channel][i] * pow(engPlusRand, i);
    
    return d_energy;
}
