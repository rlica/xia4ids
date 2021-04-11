



void correlations() {
	
	corr_unit = ref_unit;

  
  for (j=0; j<corr; j++) {

		k=0, m=0;
		i=1;
		while (k<iData) {
			l=1;
			while ( DataArray[k].chnum == firstCh[j] && 
			        DataArray[k].modnum == firstMod[j] && 
			        k+l<iData && 
			        DataArray[k].energy > 0 && 
			        DataArray[k].energy < 16384) {
				 
				if (DataArray[k+l].chnum  == secondCh[j] && 
				    DataArray[k+l].modnum == secondMod[j] && 
				    DataArray[k+l].energy > 0 && 
				    DataArray[k+l].energy < 16384 && 
				   (DataArray[k+l].time - DataArray[k].time) / (int)corr_unit < RANGE  ) {
				  
					corr_hist[j][ (DataArray[k+l].time - DataArray[k].time)/ (int)corr_unit ]++;
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
	for (j=0; j<corr; j++) {
	
		
	int ymax=0, xmax=0;
	for (z=0; z<RANGE; z++)  if (corr_hist[j][z] > ymax) { ymax = corr_hist[j][z]; xmax=z; }
	
	//fprintf(syncstat, "R%d_C%dM%d-C%dM%d: Ymax = %d at Xmax = %d\n", runnum, firstCh, firstMod, secondCh, secondMod, ymax, xmax);
	printf("R%d_M%dC%d-M%dC%d.txt|a:8\t Ymax = %d\t Xmax = %d\n", runnumber, firstMod[j], firstCh[j], secondMod[j], secondCh[j],  ymax, xmax);
	
	}
	printf("-------------------\n");
	
	
	
	
	
	
}
