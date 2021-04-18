

void write_stats() {

// printing Stats for each Run

    FILE *outfile;
    outfile=fopen(Form("n4i_Run%d.LOG",runnumber), "wt"); 

    float tot_run_time = (last_ts - first_ts)/(float)S;
    
    printf("\n---------------------------------------------------------------------------------\n");
    printf("RUN #%d    Recorded in: %.2f s        Bad/Good data chunks: %d/%d \n\n", runnumber, tot_run_time, run_missing_chunks, run_good_chunks);  
    fprintf(outfile, "RUN #%d    Recorded in:        Bad/Good data chunks: %d/%d \n\n", runnumber, tot_run_time, run_missing_chunks, run_good_chunks);  

    first_ts=0;
    printf("Module\tChannel        Good      Out-of-range   Pile-up   Rate(cps)\tComment\n\n");
    for (i=0; i<detnum; i++) {
      printf ("%6d\t%7d\t%12d %12d %12d %10.2f\t%s\n", config_coding[i][2],
	                                        config_coding[i][3], 
	                      stats[2][config_coding[i][2]][config_coding[i][3]],
	                      stats[1][config_coding[i][2]][config_coding[i][3]],
	                      stats[0][config_coding[i][2]][config_coding[i][3]],
			      stats[2][config_coding[i][2]][config_coding[i][3]] / tot_run_time,
			      comment_string[i]    );
      
      fprintf (outfile, "%6d\t%7d\t%12d %12d %12d %10.2f\t%s\n", config_coding[i][2],
	                                        config_coding[i][3], 
	                      stats[2][config_coding[i][2]][config_coding[i][3]],
	                      stats[1][config_coding[i][2]][config_coding[i][3]],
	                      stats[0][config_coding[i][2]][config_coding[i][3]],
			      stats[2][config_coding[i][2]][config_coding[i][3]] / tot_run_time,
			      comment_string[i]    );
      
       
      stats[2][config_coding[i][2]][config_coding[i][3]]=0;
      stats[1][config_coding[i][2]][config_coding[i][3]]=0;
      stats[0][config_coding[i][2]][config_coding[i][3]]=0;
    }
    
    printf("\n---------------------------------------------------------------------------------\n");
    fclose(outfile);
    

}
   
