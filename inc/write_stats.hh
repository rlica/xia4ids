

void write_stats() {

// printing Stats for each Run

    FILE *outfile;
    outfile=fopen(Form("log_Run%d.txt",runnumber), "wt"); 

    float tot_run_time = (last_ts - first_ts)/(float)S;
    
    printf("\n---------------------------------------------------------------------------------\n");
    printf("RUN #%d \nRecorded in:\t\t %.2f s \nBad/Good data chunks:\t %d/%d  \nRaw/Good signals:\t %d/%d\n\n", 
		runnumber, tot_run_time, run_missing_chunks, run_good_chunks, raw_list_size, good_list_size);  
    fprintf(outfile, "RUN #%d \nRecorded in:\t\t %.2f s \nBad/Good data chunks:\t %d/%d \nRaw/Good signals:\t %d/%d\n\n", 
		runnumber, tot_run_time, run_missing_chunks, run_good_chunks, raw_list_size, good_list_size);  

	if (run_missing_chunks > 0)
		printf("Warning: possible file integrity problem, for more information enable the 'debug' flag in xia4ids.hh\n\n");
		
    first_ts=0;
    printf("Module\tChannel        Good      Out-of-range   Pile-up   Rate(cps)\tComment\n\n");
    fprintf(outfile, "Module\tChannel        Good      Out-of-range   Pile-up   Rate(cps)\tComment\n\n");
    
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
   
