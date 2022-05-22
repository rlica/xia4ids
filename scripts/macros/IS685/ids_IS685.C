
// ///////////////////////////////////////////////////////////////////////////////////////////////
///
///
/// To compile: 
///
/// g++ ids_IS685.C `root-config --cflags` `root-config --glibs` -o ids_IS685
///
/// ///////////////////////////////////////////////////////////////////////////////////////////////


//C,C++ Libraries
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <fstream>

// ROOT libraries
#include <TChain.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TTree.h>
#include <TFile.h>
#include <TRandom3.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TLegend.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TColor.h>
#include <TROOT.h>

using namespace std;

//int main(){
int main(int argc,char* argv[]){
  
//  printf("Program Name Is: %s",argv[0]);
  if(argc==1){
    cout << "No Extra Command Line Argument Passed. Please add the name of the root file" << endl;
    return 0;
  }
  if(argc>2){
    cout << "Number Of Arguments Passed: " << argc << endl;
    cout << "This program only need 1 argument, the input root file!" << endl << endl;
      return 0;
  }
  string inputFileName = argv[1];
  cout << "Input file: " <<  inputFileName <<  endl;
  
  string outputFileName = "Ana_" + inputFileName;
  cout << "Output file: " << outputFileName <<  endl;
  
  
  TTree* t1;
  //Reset ROOT and connect tree file
  gROOT->Reset();
  TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(inputFileName.c_str());
  if (!f) {
    f = new TFile(inputFileName.c_str());
  }
  f->GetObject("ids",t1);

  //Declaration of leaves types
  Int_t           MULT;
  ULong64_t       TIME_REF;
  ULong64_t       TIMESTAMP;
  Double_t        E_Clov[24];
  Int_t           T_Clov[24];
  Int_t           M_Clov;
  Double_t        E_Beta[7];
  Int_t           T_Beta[7];
  Int_t           M_Beta;
  Double_t        E_LaBr[2];
  Int_t           T_LaBr[2];
  Int_t           M_LaBr;
  Double_t        E_TAC[4];
  Int_t           T_TAC[4];
  Int_t           M_TAC;
  Double_t        E_Proton[1];
  Int_t           T_Proton[1];
  Int_t           M_Proton;

   // Set branch addresses.
//    ids->SetBranchAddress("Multiplicity",&MULT);
//    ids->SetBranchAddress("Time_vs_ref",&TIME_REF);
//    ids->SetBranchAddress("Timestamp",&TIMESTAMP);
  t1->SetBranchAddress("Energy_Clov",E_Clov);
//    ids->SetBranchAddress("Time_Clov",T_Clov);
//    ids->SetBranchAddress("Mult_Clov",&M_Clov);
  t1->SetBranchAddress("Energy_Beta",E_Beta);
//    ids->SetBranchAddress("Time_Beta",T_Beta);
//    ids->SetBranchAddress("Mult_Beta",&M_Beta);
  t1->SetBranchAddress("Energy_LaBr",E_LaBr);
  t1->SetBranchAddress("Time_LaBr",T_LaBr);
//    ids->SetBranchAddress("Mult_LaBr",&M_LaBr);
  t1->SetBranchAddress("Energy_TAC",E_TAC);
//    ids->SetBranchAddress("Time_TAC",T_TAC);
//    ids->SetBranchAddress("Mult_TAC",&M_TAC);
//    ids->SetBranchAddress("Energy_Proton",E_Proton);
//    ids->SetBranchAddress("Time_Proton",T_Proton);
//    ids->SetBranchAddress("Mult_Proton",&M_Proton);

  TH1F* hbeta[4] ;
  for(Int_t j=0; j<4; j++) hbeta[j]  = new TH1F(Form("hbeta_%d",j),Form("hbeta_%d",j),65536,0,65536);
  TH1F* hgetot = new TH1F("hgetot","hgetot",20000,0,10000);
  TH1F* hgetot_betagated = new TH1F("hgetot_betagated","hgetot betagated",20000,0,10000);

  TH1F* hbr1= new TH1F("hbr1","hbr1",15000,0,30000);
  TH1F* hbr2= new TH1F("hbr2","hbr2",15000,0,30000);
   
  TH1F* hTbr1= new TH1F("hTbr1","hTbr1",5000,0,65536);
  TH1F* hTbr2= new TH1F("hTbr2","hTbr2",5000,0,65536);   
  TH1F* hDTbr= new TH1F("hDTbr","hDTbr",5000,-5000,5000);  
     
  TH2F* h_Br1_Br2 = new TH2F("h_Br1_Br2","h_Br1_Br2",2000,0,60000,2000,0,60000);
   
  TH1F* hTAC[4] ;
  for(Int_t j=0; j<4; j++) hTAC[j] = new TH1F(Form("hTAC_%d",j),Form("hTAC_%d",j),65000,0,65000);
   
  TH1F* hTAC3_c1 = new TH1F("hTAC3_c1","hTAC3_c1",65536,0,65536);
  TH1F* hTAC3_c2 = new TH1F("hTAC3_c2","hTAC3_c2",65536,0,65536);
   
  TH2F* h_TAC_LaBr1 = new TH2F("h_TAC_LaBr1","h_TAC_LaBr1",2000,0,60000,5000,0,20000);
  TH2F* h_TAC_LaBr2 = new TH2F("h_TAC_LaBr2","h_TAC_LaBr2",2000,0,60000,5000,0,20000);
  TH2F* h_TAC1_LaBr1 = new TH2F("h_TAC1_LaBr1","h_TAC1_LaBr1",2000,0,60000,5000,0,20000);
  TH2F* h_TAC2_LaBr2 = new TH2F("h_TAC2_LaBr2","h_TAC2_LaBr2",2000,0,60000,5000,0,20000);


  TH2F* h_TAC_Beta = new TH2F("h_TAC_Beta","h_TAC_Beta",2000,0,60000,5000,0,20000);

  Long64_t nentries = t1->GetEntries();

  cout<<"Entries "<<nentries<<endl;
   
  Double_t Emin1=750;
  Double_t Emax1=800;
  Double_t Emin2=750;
  Double_t Emax2=800;
   
  Long64_t nbytes = 0;
  
  for (Long64_t i=0; i<nentries;i++) {
    nbytes += t1->GetEntry(i);
    if(i%(Long64_t)1e6==0)cout<<"Nevents read "<<i<<endl;
    for(Int_t j=0; j<4; j++){
      if(E_Beta[j]>0)hbeta[j]->Fill(E_Beta[j]);
    }
    for(Int_t j=0; j<25; j++){
      if(E_Clov[j]>10) hgetot->Fill(E_Clov[j]);
      if(E_Clov[j]>10 && (E_Beta[0]>0 || E_Beta[1]>0 || E_Beta[2]>0))hgetot_betagated->Fill(E_Clov[j]);
    }
    
    hTbr1->Fill(T_LaBr[0]);
    hTbr2->Fill(T_LaBr[1]);
    hDTbr->Fill(T_LaBr[0]-T_LaBr[1]);
    if(E_LaBr[0]>0)hbr1->Fill(E_LaBr[0]);
    if(E_LaBr[1]>0)hbr2->Fill(E_LaBr[1]);
    
    if(E_LaBr[0]>0 && E_LaBr[1]>0)h_Br1_Br2->Fill(E_LaBr[0],E_LaBr[1]);
    
    for(Int_t j=0; j<4; j++){
      if(E_TAC[j]>10)hTAC[j]->Fill(E_TAC[j]);
    }
    
    if(E_LaBr[0]>Emin1 && E_LaBr[0]<Emax1 && E_Beta[0]>0 && E_LaBr[1]==0){
      hTAC3_c1->Fill(E_TAC[3]);
    }
    if(E_LaBr[1]>Emin2 && E_LaBr[1]<Emax2 && E_Beta[0]>0 && E_LaBr[0]==0){
      hTAC3_c2->Fill(E_TAC[3]);
    }
    //if(E_LaBr[0]>0 && E_Beta[0]>0 && E_LaBr[1]==0)h_TAC_LaBr1->Fill(E_TAC[3],E_LaBr[0]);
    
    if(E_LaBr[0]>10 && E_Beta[0]>250 && E_Beta[0]<500 && E_TAC[0]==0 && E_LaBr[1]==0) h_TAC_LaBr1->Fill(E_TAC[3],E_LaBr[0]);
    if(E_LaBr[1]>0 && E_Beta[0]>0 && E_LaBr[0]==0)h_TAC_LaBr2->Fill(E_TAC[3],E_LaBr[1]);
    if(E_LaBr[0]>0 && E_Beta[1]>0 && E_LaBr[1]==0)h_TAC1_LaBr1->Fill(E_TAC[1],E_LaBr[0]);
    if(E_LaBr[1]>0 && E_Beta[2]>0 && E_LaBr[0]==0)h_TAC2_LaBr2->Fill(E_TAC[2],E_LaBr[1]);
  if(E_LaBr[0]>1190 &&E_LaBr[0]<1230 && E_TAC[0]==0 && E_LaBr[1]==0 ) h_TAC_Beta->Fill(E_TAC[3],E_Beta[0]);
  }
  
	TFile *fout = new TFile(outputFileName.c_str(),"RECREATE");
  hgetot->Write();
  hgetot_betagated->Write();
  hbr1->Write();
  hbr2->Write();
  hTbr1->Write();
  hTbr2->Write();
  hDTbr->Write();
  for(Int_t j=0; j<4; j++)hbeta[j]->Write();
  for(Int_t j=0; j<4; j++)hTAC[j]->Write();
  
  h_Br1_Br2->Write();
  h_TAC_LaBr1->Write();
  h_TAC_LaBr2->Write();
  h_TAC1_LaBr1->Write();
  h_TAC2_LaBr2->Write();
	
  hTAC3_c1->Write();
  hTAC3_c2->Write();
  h_TAC_Beta->Write();
  fout->Close();
  
  f->Close();
	
	return 0;
	
}
