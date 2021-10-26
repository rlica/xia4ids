#include <stdio.h>
#include <stdlib.h>

#define NCHAN 8192
#define DATATYPE float

int main( int argc, char **argv){

 DATATYPE *data;
 int i, nchan;
 FILE *fileptr;
 
 if( argc != 3) {
 	printf(" Usage: mkascii <input_file> <output_file>\n");
	exit(0);
 }
 
 fileptr = fopen( argv[1], "rb");
 if( fileptr == NULL) {
    printf("  ERROR - cannot open input file : %s\n",argv[1]);
    exit(0);
    }
 data = (DATATYPE *)calloc(NCHAN, sizeof(DATATYPE));
 nchan=fread(data, sizeof(DATATYPE),NCHAN,fileptr);
 fclose(fileptr);
 fileptr = NULL;
 fileptr = fopen( argv[2], "wt");
 if( fileptr == NULL) {
    printf("  ERROR - cannot open output file : %s\n",argv[2]);
    exit(0);
    }
 for ( i = 0; i < nchan; i++)fprintf(fileptr," %d %f \n",i,data[i]);
 fclose(fileptr);
 }
 