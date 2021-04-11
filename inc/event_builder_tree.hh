
/*

 HOW does the ROOT event builder work:
 

 1. The detectors are defined in the config file. Numbering starts from 1
 2. Each distinct type represents a branch in the tree. The index corresponds to the array index (each branch is an array)  
 3. For ADDBACK there can be several detectors of the same type and index. 
 They should be calibrated because all go in the same position.
 4. There will be at least 'fold' detectors in the event. Each individual index-type combination represents a detector.
 5. Note: in ROOT numbering starts from 0.
 
 There are the following branches in the tree:
 
 E_type1[index] - ADC data
 E_type2[]
 .
 .
 T_type1[index] - (HRT) timestamp difference between each branch and the beginning of the event with offset = 1000
 T_type2[]
 .
 . 
 M_type1        - multiplicity for each branch
 M_type2
 .
 . 
 MULT     - multiplicity of the event (sum of multiplicity of each branch)
 TIME     - (LRT) timestamp difference between the event and the reference - if defined (proton pulse) 
 TIME_RUN - (LRT) timestamp in run_unit units (always)
 */




void event_builder_tree() {
  
 
  
  k=0;
  int type=0, index=0, mult=0, e=0;
  uint64_t evt_start=0, lrt_ref=0, lrt_run=0;
  
  int hrt[detnum+1];
  for(i=0; i<=detnum; i++)  hrt[i]=0;
 
  
  int detcount[dettypes+1], energy[dettypes+1][detnum+1];
  
  
  
  while ( k < iData-1) {
    
    
    m=1;
    evt_start=0;
    mult=0;
    
    for(i=1; i<=dettypes; i++) {
      for (j=1; j<=maxnum[i]; j++) energy[i][j]=0;
      detcount[i]=0;
    }
  
    
    //if we encounter a reference ADC 
    if (reftype != 0 && tmc[DataArray[k].modnum][DataArray[k].chnum] == reftype) {
      
      tref = DataArray[k].time;
      
      
      //~ //looking for the next reference
      //~ l=k+1;
      //~ while ( tmc[DataArray[l].modnum][DataArray[l].chnum] != reftype && l<iData)
      //~ l++;
      //~ 
      //~ if (tmc[DataArray[l].modnum][DataArray[l].chnum] == reftype) 
      //~ next_tref = (DataArray[l].time - tref);  
      //~ else next_tref = 0; 
      
      
      
      k++;
    }
      
    
      
    
    //finding clusters of data
    //m = number of signals inside the cluster
    while ( k+m < iData-1 && DataArray[k+m].time - DataArray[k].time < timegate ) {
	     m++;
    
      
      //if ref is inside an event we need to update the information about it
	    if (reftype !=0 && tmc[DataArray[k+m].modnum][DataArray[k+m].chnum] == reftype) {
	      tref = DataArray[k+m].time;
              //~ l=k+m+1;
              //~ //looking for the next reference
              //~ while ( tmc[DataArray[l].modnum][DataArray[l].chnum] != reftype && l<iData)
                //~ l++;
              //~ if (tmc[DataArray[l].modnum][DataArray[l].chnum] == reftype) 
                //~ next_tref = (DataArray[l].time - tref);  
              //~ else next_tref = 0; 
          }
    
    }
    
    
    
    
    //checking to see what is inside the event
    for(n=0; n<m; n++) {
      type = tmc[DataArray[k+n].modnum][DataArray[k+n].chnum]; 

    //looking for detector signals in order to calculate HRT
    //we do not calculate hrt for reftype
      if (   type  > 0  && type != reftype   )  {
        
        detcount[type]++;
        e++;
        if(evt_start == 0) 
	      evt_start = DataArray[k+n].time;
      
        
      }     
     
    }
    
    
    
    
    
    
    ////// Checking the multiplicity
    
    //there will be at least 'mult' detectors in the event
    for (i=1; i<=dettypes; i++)
      if (i != reftype) mult+=detcount[i];
    
    if (mult < fold) {
      k++;
      continue;
    }
    
    
    
      
    //extracting event information
    
    
    
    for (n=0; n<m; n++) {
       type =  tmc      [DataArray[k+n].modnum][DataArray[k+n].chnum];
      index = ntmc[type][DataArray[k+n].modnum][DataArray[k+n].chnum];
     
      if (   type > 0  /*&& index != reftype*/ )  {
	    
        // HRT: high resolution time in digitizer units
        hrt[n] = 1000 + DataArray[k+n].time - evt_start ;
        
        // Reading the energy - performing addback if we have more detectors of the same type
        //std::cout << "type: " << type << std::endl;
        //std::cout << "index: " << index << std::endl;
        //std::cout << "dettypes: " << dettypes << std::endl;
        //std::cout << "detnum: " << detnum << std::endl;
        //std::cout << "k+n = " << k+n << std::endl;
        //std::cout << "energy[0][0]: " << energy[0][0] << std::endl;
        //std::cout << "DataArray[k+n].energy: " << DataArray[k+n].energy << std::endl;
        energy[type][index] += DataArray[k+n].energy;
               
      }
    }
    
    
    // LRT: low resolution time

      //TIMESTAMP - always record this
    lrt_run = 1 + DataArray[k].time/run_unit;

      //Predefined reference (proton pulse)
    if (reftype == 0)  // we don't define any reference (proton)
        lrt_ref = 0;
    
    else if (reftype > 0 && tref == 0) //signals at the beginning of data for which we don't have reference information
        lrt_ref = 30000;
    else  //signals for which we have ref
        lrt_ref = (DataArray[k].time-tref)/ref_unit;

        
     //finished extracting information for current event
    
    
    
    


    
    
    //saving the event in ROOT TTree format 
   
    //TEventArray[iEvt].MULT  = mult;      // multiplicity
   
    //TEventArray[iEvt].TIME_REF  = lrt_ref;    // time vs last reference type
  
    //TEventArray[iEvt].TIME_RUN  = lrt_run;    // timestamp in run_unit units
    
    MULT_branch = mult;
    TIME_REF_branch = lrt_ref;
    TIME_RUN_branch = lrt_run;

   
            //NOTE: In ROOT numbering for leafs (index) starts from 0
	  for(n=0; n<m; n++) {
	    type =  tmc      [DataArray[k+n].modnum][DataArray[k+n].chnum];
     index = ntmc[type][DataArray[k+n].modnum][DataArray[k+n].chnum];
            //~if (index != reftype) {
  
              //TEventArray[iEvt].E[type][index-1] = energy[type][index];
              //TEventArray[iEvt].T[type][index-1] = hrt[n];
              //TEventArray[iEvt].M[type]        = detcount[type];
              //hrt[n] = 0;
              
              E_branch[type][index-1] = energy[type][index];
              T_branch[type][index-1] = hrt[n];
              M_branch[type] = detcount[type];
              
              hrt[n] = 0;
	      
            //~}
            
	  }
	  
	  //~ 
    //~ if (reftype > 0) {
      //~ TEventArray[iEvt].E[reftype] = next_tref/lrtunit;
      //~ TEventArray[iEvt].T[reftype] = tref/lrtunit;
    //~ }
	  
    tree->Fill();

    MULT_branch = 0;
    TIME_REF_branch = 0.0;
    TIME_RUN_branch = 0.0;
    memset(E_branch, 0, sizeof(E_branch));
    memset(T_branch, 0, sizeof(T_branch));
    memset(M_branch, 0, sizeof(M_branch));


    iEvt++;
    k+=m;
    
     
  }
} //end of event_builder_tree.h

  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  


    
    
