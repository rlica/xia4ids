
//In correlation mode, we need to delay the stop (stop = secondCh,secondMod)
//The start is always the same reference and should not be used as stop
// The time spectra are in digitizer time units (4ns)


void correlations() {
	
  
  for (j=0; j<corr_format; j++) {

		k=0, m=0;
		i=1;
        int energy_overflow = 60000;
		while (k<iData) {
			l=1;
			while ( DataArray[k].chnum == firstCh[j] && 
			        DataArray[k].modnum == firstMod[j] && 
			        k+l<iData && 
			        DataArray[k].energy > 0 && 
			        DataArray[k].energy < energy_overflow) {
				 
				if (DataArray[k+l].chnum  == secondCh[j] && 
				    DataArray[k+l].modnum == secondMod[j] && 
				    DataArray[k+l].energy > 0 && 
				    DataArray[k+l].energy < energy_overflow && 
				   (DataArray[k+l].time - DataArray[k].time) < RANGE  ) {
				  
					corr_hist[j][(DataArray[k+l].time - DataArray[k].time)]++;
					m++;
					
				}
				if ( DataArray[k+l].chnum  == firstCh[j] && 
				     DataArray[k+l].modnum == firstMod[j]  )  k=k+l-1;
				
				l++;
			}
			
			k++;
		}
		
  }
		
		
	printf("-------------------\n");
	for (j=0; j<corr_format; j++) {
	
		
	int ymax=0, xmax=0, dx=0;
	for (z=0; z<RANGE; z++)  if (corr_hist[j][z] > ymax) { ymax = corr_hist[j][z]; xmax=z; dx=CORR_DELAY-xmax; }
	
	printf("R%d_M%02dC%02d-M%02dC%02d.txt|a:64\t\t Ymax = %d\t Xmax = %d\t dX = %d \n", runnumber, firstMod[j], firstCh[j], secondMod[j], secondCh[j],  ymax, xmax, dx);
	
	}
	printf("-------------------\n");
	
	
	
	
	
	
}
