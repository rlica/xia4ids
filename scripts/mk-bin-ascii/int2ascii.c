#include <stdio.h>
#include <stdlib.h>

//#define NCHAN 1024
#define DATATYPE int

int main( int argc, char **argv){

 int NCHAN;
 DATATYPE *data;
 int i, nchan;
 FILE *fileptr;
  char outfile[100];
  
  
 if( argc != 2) {
 	printf(" Usage: mkascii <input_file> \n");
	exit(0);
 }
 
 fileptr = fopen( argv[1], "rb");
 if( fileptr == NULL) {
    printf("  ERROR - cannot open input file : %s\n",argv[1]);
    exit(0);
    }
  
  
  printf("Number of channels {1 - 16 k}:\t");
  scanf("%d", &NCHAN);
  if (NCHAN>=1 || NCHAN<=16) {
    NCHAN=NCHAN*1024;
  }
  else {
    printf(" ERROR - wrong number of channels: %d\n", NCHAN);
    exit(0);
  }
  
  
 data = (DATATYPE *)calloc(NCHAN, sizeof(DATATYPE));
 nchan=fread(data, sizeof(DATATYPE),NCHAN,fileptr);
 fclose(fileptr);
 fileptr = NULL;
 
  sprintf(outfile, "%s.ascii", argv[1]);
  fileptr = fopen(outfile, "wt");
 if( fileptr == NULL) {
    printf("  ERROR - cannot open output file : %s\n",argv[2]);
    exit(0);
    }
 for ( i = 0; i < nchan; i++)fprintf(fileptr," %d\t%d \n",i,data[i]);
 fclose(fileptr);
  
 }
 
