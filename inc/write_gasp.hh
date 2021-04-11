// GASP OUTPUT



void write_gasp() {
  


	GHeader[0].head[0]=16;
	GHeader[0].head[1]=1;
	GHeader[0].head[2]=runnumber;
	GHeader[0].head[3]=0x4758;  // HG = 0x4748, XG = 0x4758
	GHeader[0].head[4]=16;
	short int recSize=0;
	
	// 16k = 16 * 1024 bytes - gasp record lenght;
	// sizeof(short int) = 2 bytes => maximum recSize = 8*1024 words;
	
	
	fwrite (&GHeader[0], 1, 16*sizeof(short int), fp_out);
	recSize = 16;
	for (i=0; i<iEvt; i++) {
		
		if (recSize < 8*1024 - (EventArray[i].evSize+1) ) {
			fwrite (&EventArray[i], 1, (EventArray[i].evSize+1)*sizeof(short int), fp_out);
			recSize += (EventArray[i].evSize+1);
		}
		else {  fwrite (zero, 1, (8*1024-recSize)*sizeof(short int), fp_out);
		//GHeader[0].head[1] += 1;
		fwrite (&GHeader[0], 1, 16*sizeof(short int), fp_out);
		recSize = 16;
		}
	}
	fwrite (zero, 1, (8*1024-recSize)*sizeof(short int), fp_out);
	
	fflush(fp_out);
	
	
	
}