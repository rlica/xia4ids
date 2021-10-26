#include <stdio.h>
#include <stdlib.h>

#define NCHAN 1024
#define DATATYPE float

int main( int argc, char **argv){

 DATATYPE *data1, *data2;
 int i, nchan;
 FILE *fileptr;
 
 data1 = (DATATYPE *)calloc(NCHAN, sizeof(DATATYPE));
 data2 = (DATATYPE *)calloc(NCHAN, sizeof(DATATYPE));
 
 
 if( argc != 4) {
 	printf(" Usage: mkascii <input_file1> <input_file2> <output_file>\n");
	exit(0);
 }
 
 fileptr = fopen( argv[1], "rb");
 if( fileptr == NULL) {
    printf("  ERROR - cannot open input file : %s\n",argv[1]);
    exit(0);
    } 
 nchan=fread(data1, sizeof(DATATYPE),NCHAN,fileptr); 
 
 fileptr = fopen( argv[2], "rb");
 if( fileptr == NULL) {
    printf("  ERROR - cannot open input file : %s\n",argv[2]);
    exit(0);
    } 
 nchan=fread(data2, sizeof(DATATYPE),NCHAN,fileptr);  
 
 fclose(fileptr);
 fileptr = NULL;
 fileptr = fopen( argv[3], "wt");
 if( fileptr == NULL) {
    printf("  ERROR - cannot open output file : %s\n",argv[3]);
    exit(0);
    }
    
 for ( i = 0; i < nchan; i++)fprintf(fileptr," %d\t%f \n",i,data1[i]+data2[i]);
 fclose(fileptr);
 }
 
