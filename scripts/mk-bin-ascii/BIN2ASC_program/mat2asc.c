#include<stdio.h>
#include<math.h>
//#include<process.h>
//#include<stdlib.h>
#include<alloca.h>
#include<libio.h>

int g;
char *path0, *path1;
int mat[2048][2048],matsum[2048][64];
double sumsus[64],sumjos[64];
int dummy;
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

  {     long int i, j,l,k, mm1, mm2, mm3, mm4, counts;
        int sum,summat;
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
	j=0;
	for (l=0;l<2048;l++)
	  for(k=0;k<64;k++)
	    {matsum[l][k]=0;
	   }
	 for(k=0;k<64;k++) {sumsus[k]=0.;sumjos[k]=0.;}
	k=0;
	l=0;
	sum=0;
	summat=0;
	while(fscanf(p_fis0,"%c",&k1)!=EOF)
	{  
	   if(fscanf(p_fis0,"%c",&k2)!=EOF)
	     //{  if(fscanf(p_fis0,"%c",&k3)!=EOF)
	       // {  if(fscanf(p_fis0,"%c",&k4)!=EOF);}
	      //}
	   i+=1;
	   if(i==2048)
	   {i=0;j++;}
	   if((i+1)%32==0) k++;
//	   if(k==64) {k=0;if((j+1)%32==0)l++;}
	   if(k==64) {k=0;}
//	   if((j+1)%32==0) l++;
	   dummy = mm1*k2+k1;
	   matsum[j][k]=matsum[j][k]+dummy;
	   mat[j][i] = dummy;
	   sumsus[k]=sumsus[k]+(double)matsum[j][k]*j;
	   sumjos[k]=sumjos[k]+(double)matsum[j][k];
	   sum=sum+dummy;
	   
			//   counts = mm3*k4+mm2*k3+mm1*k2+k1;
//	  fprintf(p_fis1,"%6u  %6i %6i %6i %6i %12d\n",i , k1, k2, k3, k4, counts);
//	  fprintf(p_fis1,"%6u %6u %12d\n",l ,k, matsum[l][k]);
           }
	summat=0;
	for (l=0;l<=2047;l++)
	  for(k=0;k<=63;k++)
//	    {fprintf(p_fis1,"%6u %6u %d\n",l ,k, matsum[l][k]);
{
	    summat=summat+matsum[l][k];
	    //printf("%6u %6u %d %d\n",l ,k, matsum[l][k],summat);
	   }
	 for(k=0;k<=63;k++) 
	 {if(sumjos[k]!=0.) fprintf(p_fis1,"%6u %g\n",k, sumsus[k]/sumjos[k]);}
	
	fclose(p_fis0);
	fclose(p_fis1);
	printf("%d \n",sum);
	printf("%d \n",summat);
	g=0;
	printf("%d %x\n",sizeof(int),~g<<1);	
       	}
