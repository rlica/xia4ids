
#include<stdio.h>
#include<math.h>
//#include<process.h>
//#include<stdlib.h>
#include<alloca.h>
#include<libio.h>

char *path0, *path1, *path2, *fff;
int g;


void READ_CALC();



void main(int argc,char *argv[12],char envp[12])
  {  /*if(argc<2)
     {  printf("\nNumarul de Argc=%u",argc);
        printf("\nTasati nume_fisier_input nume_fisier_output");
        exit(1);
        }*/
     if((path0=(char*)calloc(80,sizeof(char)))==NULL)
     {  printf("\nMEMORIE INSUFICIENTA");
	exit(1);
	}
     if((path1=(char*)calloc(80,sizeof(char)))==NULL)
     {  printf("\nMEMORIE INSUFICIENTA");
	exit(1);
	}
     if((path2=(char*)calloc(80,sizeof(char)))==NULL)
     {  printf("\nMEMORIE INSUFICIENTA");
	exit(1);
	}
     if((fff=(char*)calloc(4,sizeof(char)))==NULL)
     {  printf("\nMEMORIE INSUFICIENTA");
	exit(1);
	}	
	
     for(g=0;g<=11;g++)
	{  *(path0+g)=(char)(*(argv[1]+g));
	   }
      for(g=0;g<=11;g++)
	{  *(path1+g)=(char)(*(argv[2]+g));
	   }
      for(g=0;g<=11;g++)
	{  *(path2+g)=(char)(*(argv[3]+g));
	   }
     READ_CALC();
     }



void READ_CALC()

  {    //subroutine reb( count,clow,enlow,chi,enhi,ste,ispe)
	int ispe,j;
        float rcount[4096],a1,a2;
        float clow,enlow,chi,enhi,ste,e,rel,reh,el,eh;
        int il,ih; 
        
        unsigned int i,k,bbb[4096],mm1,mm2,mm3,mm4;
        char cnt;
        unsigned char ddd;
        char b1,b2,b3,b4;
//        char fff[4];
//	int CNTS[4096];
        FILE *p_fis0;
	FILE *p_fis1;
	FILE *p_fis2;
	
//        float LV[20],BM1[20],BE2[20],E[20],GA[20],BR[20];
 //       float LEV,GT,LVs,BM1s,BE2s;
	
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
        
	p_fis2=fopen(path2,"r");
	if(ferror(p_fis2))
	{  perror("\nEROARE! Fisierul 2 nu poate fi deschis\n");
	   getchar();
	   clearerr(p_fis2);
           exit(1);
	   }
        


 //       fscanf(p_fis0,"%f",&LEV);
        i=0;k=0;
	while(fscanf(p_fis0,"%u\n",&k)!=EOF)
	{  i+=1;
	   //printf("%u %u\n",i,k);
           mm1=(unsigned int)(k/pow(3,256));  
           mm2=(unsigned int)((k-mm1)/pow(2,256));
           mm3=(unsigned int)((k-mm1-mm2)/256.0);
           mm4=(unsigned int)(k-mm1-mm2-mm3);
           b1=(char)mm1;b2=(char)mm2;b3=(char)mm3;b4=(char)mm4;  
           //     b1=(unsigned char)(*(fff+3));
           //     b2=(unsigned char)(*(fff+2));
           //     b3=(unsigned char)(*(fff+1));
           //     b4=(unsigned char)(*(fff));
           //     bbb[k]=b4+256*b3+pow(2,256)*b2+pow(3,256)*b1;
            fprintf(p_fis1,"%c%c%c%c",b4,b3,b2,b1);
            }
            
        
        
/*        fscanf(p_fis2,"%f %f\n",&clow,&chi);
        fscanf(p_fis2,"%f %f %f\n",&enlow,&enhi,&ste);          
        fscanf(p_fis2,"%u\n",&ispe);          
  //      clow=931.7;
   //     enlow=442.0;
        
   //     chi=1857.2;
   //     enhi=864.0;
        
   //     ste=1.5;
        
     //   ispe=4096;
	a1=(enhi-enlow)/(chi-clow);
	a2=enlow-a1*clow;


        for(i=1;i<=ispe;i++)
       {  //count[i]=(float)bbb[i];
          rcount[i]=0;
      //    printf("%f\n",(float)bbb[i]);
          }

       for(i=1;i<=ispe;i++)
       {  e=a1*((float)i)+a2;
	  el=e-a1/2.0;
	  eh=e+a1/2.0;
	  il=(int)(e/ste-a1/2.0/ste-1);
	  ih=(int)(e/ste+a1/2.0/ste+1);
          if(il<1)il=1;
          if(ih>ispe)ih=ispe;
     //     printf("%f\n",rcount[i]);  
          for(j=il;j<=ih;j++)
          {  rel=ste*((float)(j)-0.5);
	     reh=rel+ste;
	     ll=rel-el;
	     hl=reh-el;
	     lh=rel-eh;
	     hh=reh-eh;
	     if((rel<=el)&&(reh<=eh))
             {  rcount[j]=rcount[j]+(reh-el)/a1*(float)bbb[i];
                }
	     else
             {  if((rel<=el)&&(reh>=eh))
                {  rcount[j]=rcount[j]+(float)bbb[i];
                   }
		else
                {  if((rel>=el)&&(reh<=eh))
                   {  rcount[j]=rcount[j]+ste/a1*(float)bbb[i];
                      }
		   else
                   {  if((rel>=el)&&(reh>=eh))
		      {  rcount[j]=rcount[j]+(eh-rel)/a1*(float)bbb[i];
                         }
                      }
                   }
                }         
             }
          fprintf(p_fis1,"%u %u\n",i,(unsigned int)rcount[i]);           		
          }


//        for(k=1;k<=ispe;k++)
//	{  bbb[k]=(int)(ceil(rcount[k]));
	   //fprintf(p_fis1,"%f\n",rcount[k]);
 //           printf("%u   %f\n",k,rcount[k]);  
  //         } */     
	}
