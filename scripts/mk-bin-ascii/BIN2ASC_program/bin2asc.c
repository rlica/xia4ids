
#include<stdio.h>
#include<math.h>
//#include<process.h>
//#include<stdlib.h>
#include<alloca.h>
#include<libio.h>

int g;
char *path0, *path1;

void READ_CALC();



int main(int argc,char *argv[])
  {  if(argc=!2)
     {  printf("\nNumarul de Argc=%u",argc);
        printf("\nTasati:./bin2asc nume_fisier_input nume_fisier_output");
        exit(1);
        }
     if((path0=(char*)calloc(80,sizeof(char)))==NULL)
     {  printf("\nMEMORIE INSUFICIENTA");
	exit(1);
	}
     if((path1=(char*)calloc(80,sizeof(char)))==NULL)
     {  printf("\nMEMORIE INSUFICIENTA");
	exit(1);
	}
    for(g=0;g<=11;g++)
	{  *(path0+g)=(char)(*(argv[1]+g));
	   }
      for(g=0;g<=11;g++)
	{  *(path1+g)=(char)(*(argv[2]+g));
	   }
     READ_CALC();
     }



void READ_CALC()

  {     long int i, mm1, mm2, mm3, mm4, counts;
        unsigned char k1, k2, k3, k4;
        FILE *p_fis0;
	FILE *p_fis1;
	
	p_fis0=fopen(path0,"r");
	if(ferror(p_fis0))
	{  perror("\nEROARE! Fisierul 0 nu poate fi deschis\n");
	   getchar();
	   clearerr(p_fis0);
	   exit(1);
	   }
	   
	p_fis1=fopen(path1,"w");
	if(ferror(p_fis1))
	{  perror("\nEROARE! Fisierul 1 nu poate fi deschis\n");
	   getchar();
	   clearerr(p_fis1);
           exit(1);
	   }

        mm1 = 256;
	mm2 = 256*256;
	mm3 = 256*256*256;
        i=0;
	while(fscanf(p_fis0,"%c",&k1)!=EOF)
	{  if(fscanf(p_fis0,"%c",&k2)!=EOF)
	   {  if(fscanf(p_fis0,"%c",&k3)!=EOF)
	      {  if(fscanf(p_fis0,"%c",&k4)!=EOF);
	         }
	      }
	   i+=1;
	   counts = mm3*k4+mm2*k3+mm1*k2+k1;
//	   fprintf(p_fis1,"%6u  %6i %6i %6i %6i %12d\n",i , k1, k2, k3, k4, counts);
	   fprintf(p_fis1,"%6u  %12d\n",i , counts);
           }
	fclose(p_fis0);
	fclose(p_fis1);
       	}
