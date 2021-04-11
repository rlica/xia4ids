

void write_correlations() {
  
  
  
  FILE *syncstat;
  syncstat=fopen("sync-stat.txt", "wt");
  
  
  for (j=0; j<corr; j++) {
	
	    int ymax=0, xmax=0;
	    
	    sprintf(outname, "R%d_M%dC%d-M%dC%d.txt", runnumber-1, firstMod[j], firstCh[j], secondMod[j], secondCh[j]);	
	    fp_out=fopen(outname, "wt");
	    
	    for (z=0; z<RANGE; z++) {
	      fprintf(fp_out, "%d\n", corr_hist[j][z]);
	      if (corr_hist[j][z] > ymax) { ymax = corr_hist[j][z]; xmax=z; }
	      corr_hist[j][z]=0; //it's nice to zero the data for the next run
	    }
	    
	    fprintf(syncstat, "R%d_M%dC%d-M%dC%d\t Ymax = %d\t Xmax = %d\n", runnumber-1, firstMod[j], firstCh[j], secondMod[j], secondCh[j], ymax, xmax);
	    

	  }
	  
	  fflush(syncstat);
	  fclose(syncstat);
	  fflush(fp_out);
	  fclose(fp_out);

}