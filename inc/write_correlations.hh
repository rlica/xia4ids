

void write_correlations() {
  
  
  
  FILE *syncstat;
  syncstat=fopen("sync-stat.txt", "at");
  
  
  for (j=0; j<corr; j++) {
	
	    int ymax=0, xmax=0, dx=0;
	    
	    sprintf(outname, "R%d_M%02dC%02d-M%02dC%02d.txt", runnumber, firstMod[j], firstCh[j], secondMod[j], secondCh[j]);	
	    fp_out=fopen(outname, "wt");
	    
	    for (z=0; z<RANGE; z++) {
	      fprintf(fp_out, "%d\n", corr_hist[j][z]);
	      if (corr_hist[j][z] > ymax) { ymax = corr_hist[j][z]; xmax=z; dx=CORR_DELAY-xmax; }
	      corr_hist[j][z]=0; //it's nice to zero the data for the next run
	    }
	    
	    fprintf(syncstat, "R%d_M%02dC%02d-M%02dC%02d\t\t Ymax = %10d\t\t Xmax = %10d\t\t dX = %10d \n", runnumber, firstMod[j], firstCh[j], secondMod[j], secondCh[j], ymax, xmax, dx);
	    

	  }
	  
	  fflush(syncstat);
	  fclose(syncstat);
	  fflush(fp_out);
	  fclose(fp_out);

}
