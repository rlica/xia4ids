
/*

 "Our standard format for these 4 numbers is 4 x (integer*2), which means 4 x (16 bits, binary format).
 The maximum number for each parameter is 32767, and any number above it must be set to 32767." (Henryk)
 
 
 
  proton-time(ms), beta-energy, labr-energy, TAC                                                  - 4 entries
 
 (proton)-(beta_energy)-(labr1_energy)-(fastTAC_beta_Labr1)-(Labr2_energy)-(fastTAC_labr1_labr2)  - 6 entries
 
 (proton)-(beta_energy)-(ge_energy)-[slowTAC(beta-Ge)]-(labr_energy)-(ftac(beta-labr))            - 6 entries
 
 
 HOW does the event builder work:
 
 1. The event is formed by several entries (see above).
 
 2. The detectors are defined in the config file.
    Each distinct type represents an entry in the event. 
    There can be several detectors of the same type. They should be calibrated because they go in the same entry.
    The type ordering in the config file is the ordering of the entries in the event.
 
 3. To write the "timestamp difference" between current entry and a common reference
 inside the event, set the type of the reference at "Link" for each detector. 
      The timestamp difference entry will appear after the energy entry.
 
 
 */




void event_builder_list() {
  
 
    //Here we define the size of the event, not including the zero separator
    int evSize = list_evSize;
    
    
  k=0;
  int index;
  uint64_t sum=0, lrt;
  int hrt[detnum+1], reference_hrt=0;
  for(i=0; i<=detnum; i++) hrt[i]=0;
  int detcount[dettypes+1];
  
  
  
  
  
  
  
	   
  while ( k < iData-1) {
    
    
    m=1;
    e=0, sum=0;
    for(i=0; i<=dettypes; i++) detcount[i]=0;
    
    
    
    //if we encounter a reference ADC 
    if (reftype != 0 && tmc[DataArray[k].modnum][DataArray[k].chnum] == reftype) {
      tref = DataArray[k].time;
      k++;
    }
      
    
      
    
    //finding clusters of data
    //m = number of signals inside the cluster
    while ( k+m < iData-1 && DataArray[k+m].time - DataArray[k].time < timegate ) {
	     m++;
    
      
      //if ref is inside an event we need to update the information about it
	    if (reftype !=0 && tmc[DataArray[k+m].modnum][DataArray[k+m].chnum] == reftype) {
	      tref = DataArray[k+m].time;
          }
    
    }
    
    
    
    
    //checking to see what is inside the event
    for(n=0; n<m; n++) {

    //looking for detector signals in order to calculate HRT
      if (   tmc[DataArray[k+n].modnum][DataArray[k+n].chnum] > 0 &&
             tmc[DataArray[k+n].modnum][DataArray[k+n].chnum] != reftype )  {
        
        
        detcount[tmc[DataArray[k+n].modnum][DataArray[k+n].chnum]]++;
        sum += DataArray[k+n].time;
        e++;
        
      }     
     
    }
    
    
    
    
    
    
    ////// Checking to see if we have too few or too many signals
    
    
      for ( i=1; i<=dettypes; i++ ) 
        if ( i != reftype && detcount[i] == 0 ) break;
    
      if (i<=dettypes) {
        k++;
        continue;
      }
      
    
    

    
    
    
      
    //extracting timing information (HRT and LRT)
    
    for (n=0; n<m; n++) {
      if (   tmc[DataArray[k+n].modnum][DataArray[k+n].chnum] > 0 &&
             tmc[DataArray[k+n].modnum][DataArray[k+n].chnum] != reftype )  {
	    
        // HRT: high resolution time in digitizer units
        hrt[n] = DataArray[k+n].time - sum/e + 1000;
        
        // Finding the reference detector for HRT
        if (reference_link == tmc[DataArray[k+n].modnum][DataArray[k+n].chnum]) 
          reference_hrt = hrt[n];
        
        
      }
        
        // LRT: low resolution time
        if (reftype == 0)  // we take the timestamp
            lrt = 1 + DataArray[k].time/run_unit;
        
        else if (reftype > 0 && tref == 0) //signals at the beginning of data for which we don't have reference information
            lrt = 8000;
        else  //signals for which we have ref
            lrt =    (DataArray[k].time-tref)/ref_unit;
        
       
	}//finished extracting information for current event
    
    
    
    //saving the event in List format (numbering starts from 0)
    //in this code numbering starts from 1 for the type and detector index.
    
  
    
    index = 0;
    EventArray[iEvt].elem[index++] = 0;         // 0 = header List Event
    if (reftype!=0) {
      EventArray[iEvt].elem[index++] = lrt;       // time vs last reference type
    }
    
    
    for(j=1; j<=dettypes; j++) 
      if ( j!=reftype )
          for(n=0; n<m; n++)
            if (tmc[DataArray[k+n].modnum][DataArray[k+n].chnum] == j) {
  
              EventArray[iEvt].elem[index++] = DataArray[k+n].energy;
              
              if (link_type[j]!=0) 
                EventArray[iEvt].elem[index++] = CORR_DELAY + hrt[n] - reference_hrt;
                
              break; //using break, we write only one detector from the same type
            }
    
    
    EventArray[iEvt].evSize  = evSize;    // evSize does not include the separator
            
    if (index != evSize+1) {
        printf("ERROR: Event size different than %d. Check 'Link' in config file \n", evSize);
        exit(0);
    }
    
    iEvt++;
    k+=m;
    
    
    for(i=0; i<m; i++)   hrt[i]=0;
    reference_hrt = 0;

    
  }
  
} //end of event_builder_list.h

  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  


    
    
