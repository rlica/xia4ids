
/* Writing Completion time
 
 We assume processing speed is constant:
 
 
    current_block    full_size - current_block
s = ------------- = --------------------------- 
      elapsed               remaining

 
 */


void write_time() {
  
  
  double elapsed=0, remaining=0; //in seconds
  int remaining_min, remaining_sec;
  
  elapsed = ((double)clock() - start_clock) / CLOCKS_PER_SEC;
     
  remaining = (full_size - current_block)*elapsed/current_block;
  
  remaining_min = (int)remaining / 60 ;
  remaining_sec = (int)remaining % 60 ;
  
  printf(" [ %d%% - %2dm%2ds ]", 100*current_block/full_size, remaining_min, remaining_sec);
  

}