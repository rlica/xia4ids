{
//////////////////////////////////////////////////////////
//   This file has been automatically generated 
//     (Sat Apr  9 00:07:19 2022 by ROOT version6.18/04)
//   from TTree ids/Isolde Decay Station Tree
//   found on file: Run100.root
//////////////////////////////////////////////////////////


//Reset ROOT and connect tree file
   gROOT->Reset();
   TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("Run100.root");
   if (!f) {
      f = new TFile("Run100.root");
   }
    f->GetObject("ids",tree);

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
   ids->SetBranchAddress("Multiplicity",&MULT);
   ids->SetBranchAddress("Time_vs_ref",&TIME_REF);
   ids->SetBranchAddress("Timestamp",&TIMESTAMP);
   ids->SetBranchAddress("Energy_Clov",E_Clov);
   ids->SetBranchAddress("Time_Clov",T_Clov);
   ids->SetBranchAddress("Mult_Clov",&M_Clov);
   ids->SetBranchAddress("Energy_Beta",E_Beta);
   ids->SetBranchAddress("Time_Beta",T_Beta);
   ids->SetBranchAddress("Mult_Beta",&M_Beta);
   ids->SetBranchAddress("Energy_LaBr",E_LaBr);
   ids->SetBranchAddress("Time_LaBr",T_LaBr);
   ids->SetBranchAddress("Mult_LaBr",&M_LaBr);
   ids->SetBranchAddress("Energy_TAC",E_TAC);
   ids->SetBranchAddress("Time_TAC",T_TAC);
   ids->SetBranchAddress("Mult_TAC",&M_TAC);
   ids->SetBranchAddress("Energy_Proton",E_Proton);
   ids->SetBranchAddress("Time_Proton",T_Proton);
   ids->SetBranchAddress("Mult_Proton",&M_Proton);

//     This is the loop skeleton
//       To read only selected branches, Insert statements like:
// ids->SetBranchStatus("*",0);  // disable all branches
// TTreePlayer->SetBranchStatus("branchname",1);  // activate branchname

   Long64_t nentries = ids->GetEntries();

   Long64_t nbytes = 0;
//   for (Long64_t i=0; i<nentries;i++) {
//      nbytes += ids->GetEntry(i);
//   }
}
