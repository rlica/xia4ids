//Calibrating each signal, using random number generator
//If the signal has no calibration defined, the energy will be the same

double calibrate(int module, int channel, int energy) {

    // If no calibration for channel, return ADC energy
    if (!have_cal == 0 || calib[module][channel][0] == 0) return energy;
	  
    // Generate random number between 0 and 1, and add to ADC channel
    // to remove binning issues
    static std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(-0.5,0.5);
    double engPlusRand = energy + distribution(generator);
    
    // Apply calibration and return energy
    double d_energy = 0.;
    for(int i=0; i<MAX_CAL; i++) d_energy += calib[module][channel][i] * pow(engPlusRand, i);
    
    return d_energy;
}
