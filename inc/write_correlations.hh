

void write_correlations() {
  
  
  
    FILE *syncstat;
    syncstat=fopen("sync-stat.txt", "at");


    for (j=0; j<corr_format; j++) {
      
        int ymax=0, xmax=0, dx=0;

        outname = "R" + std::to_string(runnumber) + "_M" + std::to_string(firstMod[j]) + "C" + std::to_string(firstCh[j]) + "-M" + std::to_string(secondMod[j]) + "C" + std::to_string(secondCh[j])+ ".txt";
        fp_out=fopen(outname.c_str(), "wt");

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
