
/* Writing Completion time
 
 We assume processing speed is constant:
 
 
     current_pos      full_size - current_pos
s = ------------- = --------------------------- 
      elapsed               remaining

 
 */


void printProgress(double percentage) {
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    // fflush(stdout);
}

void write_time(int current_block, int full_size) {
  
  
  double elapsed=0, remaining=0; //in seconds
  int remaining_min, remaining_sec;
  
  elapsed = ((double)clock() - start_clock) / CLOCKS_PER_SEC; 
     
  remaining = (full_size - current_block)*elapsed/current_block;
  
  remaining_min = (int)remaining / 60 ;
  remaining_sec = (int)remaining % 60 ;
  
  //printf(" [ %d%% - %2dm%2ds ]", 100*float(current_block)/float(full_size), remaining_min, remaining_sec);
  //printf(" [ %d MB/s - %2dm%2ds ]", (float(current_block)/1048576)/float(elapsed), remaining_min, remaining_sec);
  printf(" [ %2dm%2ds remaining ]", remaining_min, remaining_sec);
  

}
