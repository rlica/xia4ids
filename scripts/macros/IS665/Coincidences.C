#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TMath.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TTreeIndex.h"
#include "fstream"
#include <cmath>


class Coincidences {
public :
	Int_t crystals;
	Int_t silicons;
	
	TFile* f;
	TTree* tree;
	
	Int_t MULT; 				//Multiplicity
	ULong64_t TIME_REF;			//Time vs ref
	ULong64_t TIMESTAMP;		//Timestamp
	Int_t E_Clov[16];			//Energy HPGe
	Int_t T_Clov[16];			//Time HPGe
	Int_t M_Clov;				//Multiplicity HPGe
	Int_t E_Si[8];		//Energy SPEDE
	Int_t T_Si[8];			//Time SPEDE
	Int_t M_Si;				//Multiplicity SPEDE
	Int_t E_Beta[2];			//Energy beta
	Int_t T_Beta[2];			//Time beta
	Int_t M_Beta;				//Multiplicity beta
	Int_t E_Proton_T1[1];		//Energy PP
	Int_t T_Proton_T1[1];		//Time PP
	Int_t M_Proton_T1;			//Multiplicity PP
	
	TH1F* histg;	//single-gamma 1D histogram
	TH1F* histb;	//single-beta 1D histogram
	TH1F* histbg;	//beta-gamma 1D histogram
	TH2F* histbg2d;	//beta-gamma 1D histogram
	TH1F* histe;	//single-silicon 1D histogram
	
	TH2F* histgg;	//coincidence gamma-gamma 2D histogram
	TH2F* histbgg;	//coincidence beta-gamma-gamma 2D histogram
	TH2F* histge;	//coincidence gamma-electron 2D histogram
	TH2F* histbge;	//coincidence beta-gamma-electron 2D histogram 
	
	TH2F* histges[8]; //coincidence gamma-electron(segment) 2D histogram

	TH2F* histee; 			//coincidence electron-electron 2D histogram

	Coincidences();
	virtual ~Coincidences();
	virtual void Sort();
 };

Coincidences::Coincidences() {
	//f = new TFile("../178Au_g_1.root"); //CHANGE HERE
	f = (TFile*)gROOT->GetFile();
	crystals = 16;
	silicons = 8;
	tree = (TTree*)f->Get("ids");
	/*TChain* chain = new TChain("ids","");
	chain->Add("Run231.root/ids");*/
	
	//tree =chain;
	
	//tree = new TTree("ids","./Run231.root");
	
	if(tree == 0) return;
	
	tree->SetMakeClass(1); //this is some magic I really don't understand but it MUST be here, otherwise the code doesn't work
	tree->SetBranchStatus("*", 1);
	tree->SetBranchAddress("Multiplicity",&MULT);
	tree->SetBranchAddress("Time_vs_ref",&TIME_REF);
	tree->SetBranchAddress("Timestamp",&TIMESTAMP);
	tree->SetBranchAddress("Energy_Clov",&E_Clov);
	tree->SetBranchAddress("Time_Clov",&T_Clov);
	tree->SetBranchAddress("Mult_Clov",&M_Clov);
	tree->SetBranchAddress("Energy_Beta",&E_Beta);
	tree->SetBranchAddress("Time_Beta",&T_Beta);
	tree->SetBranchAddress("Mult_Beta",&M_Beta);
	tree->SetBranchAddress("Energy_Si",&E_Si);
	tree->SetBranchAddress("Time_Si",&T_Si);
	tree->SetBranchAddress("Mult_Si",&M_Si);
	tree->SetBranchAddress("Energy_Proton",&E_Proton_T1);
	tree->SetBranchAddress("Time_Proton",&T_Proton_T1);
	tree->SetBranchAddress("Mult_Proton",&M_Proton_T1);
}

Coincidences::~Coincidences() {
	if (!tree) return;
}

void Coincidences::Sort() {
	if(tree == 0) return;

	Long_t n = tree->GetEntries();
	char name[200];
	char title[200];

	sprintf(title,"Single-#gamma spectrum;Energy (keV);Counts / keV");
	histg = new TH1F("g",title,8192,0.,8192.);
	
	sprintf(title,"Single-#beta spectrum;Energy (keV);Counts / keV");
	histb = new TH1F("b",title,8192,0.,8192.);
	sprintf(title,"#beta-#gamma coincidence spectrum;Energy (keV);Counts / keV");
	histbg = new TH1F("bg",title,8192,0.,8192.);
	
	sprintf(title,"Single-silicon spectrum;Energy (keV);Counts / keV");
	histe = new TH1F("e",title,16384,0.,16384.);
	
	sprintf(title,"silicon-silicon coincidence spectrum;Energy (keV);Energy (keV);Counts / keV");
	histee = new TH2F("ee",title,4096,0.,16384.,4096,0.,16384.);
	
	
	sprintf(title,"#beta-#gamma coincidence spectrum;Energy (keV);Energy (keV);Counts / keV");
	histbg2d = new TH2F("bg2d",title,8192,0.,8192.,8192,0.,8192.);
	
	sprintf(title,"#gamma-#gamma coincidence spectrum;Energy (keV);Energy (keV);Counts / keV");
	histgg = new TH2F("gg",title,8192,0.,8192.,8192,0.,8192.);
	sprintf(title,"#beta-#gamma-#gamma coincidence spectrum;Energy (keV);Energy (keV);Counts / keV");
	histbgg = new TH2F("bgg",title,8192,0.,8192.,8192,0.,8192.);
	
	
	sprintf(title,"#gamma-silicon coincidence spectrum;Energy (keV);Energy (keV);Counts / keV");
	//histge = new TH2F("ge",title,8192,0.,8192.,4096,0.,16384.);
	histge = new TH2F("ge",title,4096,0.,16384.,8192,0.,8192.);
	sprintf(title,"#beta-#gamma-silicon coincidence spectrum;Energy (keV);Energy (keV);Counts / keV");
	histbge = new TH2F("bge",title,8192,0.,8192.,4096,0.,16384.);

	for(int i = 0; i < silicons; i++) {
		sprintf(name,"ges_%i",i);
		sprintf(title,"#gamma-silicon %i coincidence spectrum;Energy (keV);Energy (keV);Counts / keV",i);
		histges[i] = new TH2F(name,title,8192,0.,8192.,4096,0.,16384.);
	}

	Long_t progress = 0;
	Long_t tms = 0;
	
	for(Long_t i = 0; i < n; i++) {
		if(progress != i*100/n) {
			cout << "Progress: " << i*100/n << "% sorted" << endl;
			progress = i*100/n;
		}
		if(i == 0) tms = TIMESTAMP;
		//ULong64_t l = tree->LoadTree(index->GetIndex()[i]);
		//tree->GetEntry(l);
		tree->GetEntry(i);
		
		//single-gamma histogram
		if(M_Clov != 0) {
			for(int j = 0; j < crystals; j++) { 
				if(E_Clov[j] != 0) {
					histg->Fill(E_Clov[j]);
				}
			}
		}

		//electron-electron and single-electron histograms 
		if(M_Si != 0) {			
			for(int j = 0; j < silicons; j++) {
				if(E_Si[j] == 0) continue;
				histe->Fill(E_Si[j]);
				for(int k = 0; k < j; k++) { 
					if(E_Si[k] != 0) {
						histee->Fill(E_Si[j],E_Si[k]);
						histee->Fill(E_Si[k],E_Si[j]);			
					}
				}
			}
		}
			
		if(M_Beta != 0) {
			histb->Fill(E_Beta[0]);
		}

		//gamma-gamma & beta-gamma-gamma coincidence histograms
		if(M_Clov > 1) {
			if(M_Beta != 0) {
				for(int j = 0; j < crystals; j++) { for(int k = 0; k < j; k++) {
					if(E_Clov[j] != 0 && E_Clov[k] != 0) {
						histgg->Fill(E_Clov[j],E_Clov[k]);
						histgg->Fill(E_Clov[k],E_Clov[j]);
						histbgg->Fill(E_Clov[j],E_Clov[k]);
						histbgg->Fill(E_Clov[k],E_Clov[j]);
					}
				}}
			}
			else {
				for(int j = 0; j < crystals; j++) { for(int k = 0; k < j; k++) {
					if(E_Clov[j] != 0 && E_Clov[k] != 0) {
						histgg->Fill(E_Clov[j],E_Clov[k]);
						histgg->Fill(E_Clov[k],E_Clov[j]);
					}
				}}
			}
		}
		
		
		//beta-gamma histograms
		if(M_Beta != 0 && M_Clov != 0) {
			for(int j = 0; j < crystals; j++) {
				if(E_Clov[j] != 0) {
					histbg->Fill(E_Clov[j]);
					histbg2d->Fill(E_Clov[j],E_Beta[0]);
				}
			}
		}
		
		//gamma-electron and beta-gamma-electron coincidence histograms
		if(M_Clov != 0 && M_Si != 0) {
			if(M_Beta != 0) {
				for(int j = 0; j < crystals; j++) { for(int k = 0; k < silicons; k++) { 
					if(E_Clov[j] != 0 && E_Si[k] != 0) {
						//histbge->Fill(E_Clov[j],E_Si[k]);
						//histge->Fill(E_Clov[j],E_Si[k]);
						//histges[k]->Fill(E_Clov[j],E_Si[k]);

						histbge->Fill(E_Si[k],E_Clov[j]);
						histge->Fill(E_Si[k],E_Clov[j]);
						histges[k]->Fill(E_Si[k],E_Clov[j]);
					}
				}}
			}
			else {
				for(int j = 0; j < crystals; j++) { for(int k = 0; k < silicons; k++) { 
					if(E_Clov[j] != 0 && E_Si[k] != 0) {
						//histge->Fill(E_Clov[j],E_Si[k]);
						//histges[k]->Fill(E_Clov[j],E_Si[k])
						
						histge->Fill(E_Si[k],E_Clov[j]);
						histges[k]->Fill(E_Si[k],E_Clov[j]);
					}
				}}
			}
		}
		
	}
	

	sprintf(title,"sorted_178Au_g.root"); //CHANGE HERE

	TFile* file = new TFile(title, "RECREATE");
	file->cd();
	
	histe->Write();

	histg->Write();
	histb->Write();
	histbg->Write();
	histbg2d->Write();

	histgg->Write();
	histbgg->Write();
	histge->Write();
	histbge->Write();
	
	histee->Write();
	
	for(int i = 0; i < silicons; i++) {
		histges[i]->Write();
	}

	file->Close();
}
