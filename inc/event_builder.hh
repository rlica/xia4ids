

//11.08.2014 - new header
//             header_run  - Timestamp in run units
//	   (x) header_diff - Difference between proton pulses (commented now, not that useful) 
// If reftype == 0, there will be no LRT in the event


///  Reconstructing Events


void event_builder() {
  
 
  k=0;
  int index;
  int evSize=0;
  uint64_t sum=0;
  uint16_t hrt[detnum+1], 
           lrt_run = 1, lrt_ref = 1,
	   tac[detnum+1],
	   pair_start[detnum+1],
	   pair_stop[detnum+1],
           detcount[dettypes+1]; 
	  
  
  for(i=0; i<=detnum; i++) {
	  hrt[i]=0;
	  tac[i]=1;
	  pair_start[i]=1;
	  pair_stop[i]=1;
        }
  
  
  
	   
  while ( k < iData-1) {
    
    
    
    //if the first signal we encounter is a reference ADC 
    if (reftype != 0 && tmc[DataArray[k].modnum][DataArray[k].chnum] == reftype) {
      
      tref = DataArray[k].time;
      //~l=k+1;
      //~ //finding the difference between two ref
      //~ while ( tmc[DataArray[l].modnum][DataArray[l].chnum] != reftype && l<iData)  l++;
      //~ 
      //~ if (tmc[DataArray[l].modnum][DataArray[l].chnum] == reftype) 
      //~ header_diff = (DataArray[l].time - tref) / ref_unit;  
      //~ else header_diff = 8000; 
      
      k++;
    }
    
          
    
    
      m=1;
      e=0, sum=0;
      
      //finding clusters of data
      //m = number of signals inside the cluster
      while ( k+m < iData-1   &&
	      DataArray[k+m].time - DataArray[k].time < timegate ) {
	
	//Fix the energy to be not higher than 16000:
	if (DataArray[k+m].energy > 16383) DataArray[k+m].energy = 16383; 
      
	m++;
        
        
        //if ref is inside an event we need to update the information about it
	if (reftype !=0 && tmc[DataArray[k+m].modnum][DataArray[k+m].chnum] == reftype) {
	  tref = DataArray[k+m].time;
          //~l=k+m+1;
          //~ //finding the difference between two ref
          //~ while ( tmc[DataArray[l].modnum][DataArray[l].chnum] != reftype && l<iData)  l++;
          //~ if (tmc[DataArray[l].modnum][DataArray[l].chnum] == reftype) 
            //~ header_diff = (DataArray[l].time - tref) / lrtunit;  
          //~ else header_diff = 8000;  
        }
      
      }
      
	
    
             
      //checking to see what is inside the event
      for(n=0; n<m; n++) {
	
	//looking for detector signals in order to calculate HRT
        if (   tmc[DataArray[k+n].modnum][DataArray[k+n].chnum] > 0         &&  
	       tmc[DataArray[k+n].modnum][DataArray[k+n].chnum] != cs_tac   && 
	       tmc[DataArray[k+n].modnum][DataArray[k+n].chnum] != flagtype     )  {
	  
          sum += DataArray[k+n].time;
          e++;
          
        }     
       
      }
      
    
    
      //checking the coincidence level (fold)
      if (e < fold) {
	k++;
	continue;
      }
      
      
        //extracting timing information (HRT and LRT)
        for (n=0; n<m; n++) {
          if ( tmc[DataArray[k+n].modnum][DataArray[k+n].chnum] != cs_tac && 
	       tmc[DataArray[k+n].modnum][DataArray[k+n].chnum] != flagtype &&
	       tmc[DataArray[k+n].modnum][DataArray[k+n].chnum] > 0   ) {
	    
	    // HRT: high resolution time in digitizer units
	    // a single event will have HRT=1000
	    hrt[n] = DataArray[k+n].time - sum/e + 1000;
	  
	    
	    //pair_tac will be treated as a detector with two extra parameters
	    //the parameters are the start and stop energies
	    if (pair_tac!=0 && tmc[DataArray[k+n].modnum][DataArray[k+n].chnum] == link_type[pair_tac] ) {
	      index = ntmc[link_type[pair_tac]] [DataArray[k+n].modnum] [DataArray[k+n].chnum];
	      for (i=1; i<=maxnum[pair_tac]; i++) {
		if (start[i][pair_tac] == index)        //from config: start[index][dettype]
		  pair_start[i] = DataArray[k+n].energy;
		if (stop[i][pair_tac] == index)
		  pair_stop[i] = DataArray[k+n].energy;
	      }
	    }
	    
          }
        
          //cs_tac (common start) is a parameter for each stop detector
          else if (cs_tac!=0 && tmc[DataArray[k+n].modnum][DataArray[k+n].chnum] == cs_tac) {
	    index = ntmc[cs_tac][DataArray[k+n].modnum][DataArray[k+n].chnum];
            //11.09.2015
	    //if the same mod+ch+type is defined as multiple indexes,
	    //the index will be 10203 (if defined as 1,2,3) or 141516(if 14,15,16)
	    while (index%100>0) { 
	      tac[index%100] = DataArray[k+n].energy;
	      index = index / 100;
	    }

	    
	  }
	      
          else if (flagtype !=0 && tmc[DataArray[k+n].modnum][DataArray[k+n].chnum] == flagtype) {
	    index = ntmc[flagtype][DataArray[k+n].modnum][DataArray[k+n].chnum];
	    flag = index*10;  //we should have at least 2 flags for this to make sense
	  }	              //eg: tape move / tape stop, beam on / beam off...
                              //the flag parameter will be 10, 20, ... depending on the flag index  
            
	}
    
    // // LRT: low resolution time


    // //Always take the timestamp in run_unit units
    // lrt_run = 1 + DataArray[k].time/run_unit;
    // if (lrt_run > 8192) { //Fix LRT not higher than 8192:
    //   printf("\n LRT Overflow Warning: Run timestamp > 8192 - Increase the run_unit value\n");
    //   lrt_run = 8191;
    // }

    // //Reference vs proton pulse
    // if (reftype == 0)  // we don't take the reference time
    //   lrt_ref = 0;
    // else if (reftype > 0 && tref == 0)   //signals at the beginning of data for which we don't have reference information
    //   lrt_ref = 8191;
    // else    //signals for which we have ref
    //   lrt_ref =    (DataArray[k].time-tref)/ref_unit;
    
    //  if (lrt_ref > 8192) { //Fix LRT not higher than 8192:
    //    //printf("\n LRT Overflow Warning: Reference time > 8192 - Increase the ref_unit value\r");
    //    lrt_ref = 8191;
    //  } 
      
    
      //finished extracting information for current event
      
      
      //saving the event in GASPWare format (numbering starts from 0)
      //in this code numbering starts from 1 for the type and detector index.
      
      evSize = 1+1+dettypes; //includes GASPWare separator (1), headers (1) and (dettypes) detector types
      
      //removing the types that will not be treated as detectors
      if( reftype != 0) evSize--;
      if(  cs_tac != 0) evSize--;
      if(flagtype != 0) evSize--;
      
      
      
      if(evSize <= 2) {
	printf("ERROR: No detector types in the config file to be converted. (evSize = %d)\n", evSize);
	exit(0);
      }
      
      for (j=0; j<=dettypes; j++) detcount[j]=0;  //required for GASPWare to keep track of the detectors in the event
      
      for(j=1; j<=dettypes; j++) 
	if (j!=cs_tac && j!=flagtype && j!=reftype)
          for(i=1; i<=maxnum[j]; i++)   
            for(n=0; n<m; n++)
              if (ntmc[j][DataArray[k+n].modnum][DataArray[k+n].chnum] == i)  {
	      
                EventArray[iEvt].elem[evSize++] = i-1;                      //the index of the detector starting from 0
                EventArray[iEvt].elem[evSize++] = DataArray[k+n].energy;    //parameter 0
                EventArray[iEvt].elem[evSize++] = hrt[n];                //parameter 1
                if ( reftype > 0 )
		EventArray[iEvt].elem[evSize++] = lrt_ref;               //parameter 2
                if ( link_type[cs_tac] == j )
		  EventArray[iEvt].elem[evSize++] = tac[i];              //parameter 3 (optional)
                if ( pair_tac == j ) {
		  EventArray[iEvt].elem[evSize++] = pair_start[i];       //parameter 3 (optional)
		  EventArray[iEvt].elem[evSize++] = pair_stop[i];        //parameter 4 (optional)
	        }            
                if ( flagtype!=0 )
		  EventArray[iEvt].elem[evSize++] = flag;                   //parameter 4 or 5 (optional)
                detcount[j]++;
                break;
            }
            
      if (evSize > 200) {
	printf("ERROR: event_builder.h - evSize = %d (>200 !).\n", evSize);
	exit(0);
      }

      index = 0;
      EventArray[iEvt].evSize = evSize-1; // does not include the separator

      EventArray[iEvt].elem[index++] = 0xf000 + EventArray[iEvt].evSize; // separator GaspEvent
            
      EventArray[iEvt].elem[index++] = lrt_run;  //Gasp Event Header - Timestamp in run units
      //~EventArray[iEvt].elem[index++] = header_diff;  
       
      for (j=1; j<=dettypes; j++)
	if (j!=cs_tac && j!=flagtype && j!=reftype) 
	  EventArray[iEvt].elem[index++] = detcount[j];
	  
      iEvt++;
      k+=m;
    
    
    
    
    
    //zeroing arrays for the next event:
    
    for (i=0; i<m; i++)
      hrt[i]=0;
    
    if(cs_tac!=0)
      for (i=0; i<=maxnum[link_type[cs_tac]]; i++)
        tac[i]=1;
    
    if (pair_tac!=0) 
      for (i=0; i<=maxnum[pair_tac]; i++) {
        pair_start[i]=1;
        pair_stop[i]=1;
        
    }
    
    
    
  }
  
} //end of event_builder.h

  
  
