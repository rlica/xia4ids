// List OUTPUT



void write_list() {
  
	
	
	for (i=0; i<iEvt; i++)
        fwrite (&EventArray[i], 1, (EventArray[i].evSize+1)*sizeof(short int), fp_out);
	
	fflush(fp_out);
	
	
	
}