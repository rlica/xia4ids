
void TIME_CloverSum_gate(){


  gROOT->Reset();
  gStyle->SetOptStat(1000000);
  
  
  TChain ids("ids");
  ids.Add("Run020.root");
  //ids.Add("Run046.root");
  //ids.Add("Run047.root");
  
  
  const char *gateL = "980";
  const char *gateR = "990";



  TH1I *hTemp = new TH1I("hTemp","",8000,1,8000);
  TH1I *hSum_gated =  new TH1I("hSum_gated" ,Form("Gated TIME Distribution of Clover Sum - %s-%s (keV)",gateL, gateR), 8000,1,8000);
  TH1I *hSum       =  new TH1I("hSum" ,"Clover Sum",8000,1,8000);
  
  TCanvas *Can1;
  Can1 = new TCanvas("Can1","Can1",800,800); 
  Can1->Divide(1,2);

  /////GATED SUM
 
  
  
  TPad *p1 = (TPad *)(Can1->cd(1)); 
 
  cout << "Reading gated Clover 1\n";
  ids.Draw("TIME>>hTemp",Form("E_Clov1 > %s && E_Clov1 < %s", gateL, gateR));
  hSum_gated->Add(hTemp);
  
  printf("Reading gated Clover 2\n");
  ids.Draw("TIME>>hTemp",Form("E_Clov2 > %s && E_Clov2 < %s", gateL, gateR));
  hSum_gated->Add(hTemp);
  
  printf("Reading gated Clover 3\n");
  ids.Draw("TIME>>hTemp",Form("E_Clov3 > %s && E_Clov3 < %s", gateL, gateR));
  hSum_gated->Add(hTemp);
   
  printf("Reading gated Clover 4\n");
  ids.Draw("TIME>>hTemp",Form("E_Clov4 > %s && E_Clov4 < %s", gateL, gateR));
  hSum_gated->Add(hTemp);
   
  
  hSum_gated->Draw();
  printf("Finished drawing gated TIME distribution of the Clover Sum\n\n");
  
  ///////SUM 
  TPad *p2 = (TPad *)(Can1->cd(2)); 
  
  printf("Reading Clover 1\n");
  ids.Draw("E_Clov1>>hTemp");
  hSum->Add(hTemp);
  
  printf("Reading Clover 2\n");
  ids.Draw("E_Clov2>>hTemp");
  hSum->Add(hTemp);
  
  printf("Reading Clover 3\n");
  ids.Draw("E_Clov3>>hTemp");
  hSum->Add(hTemp);
   
  printf("Reading Clover 4\n");
  ids.Draw("E_Clov4>>hTemp");
  hSum->Add(hTemp);
   
  hSum->SetLineColor(2);
  hSum->Draw();
  printf("Finished drawing Clover Sum\n\n");
  
  	Can1->ToggleToolBar();
	Can1->SetGridx();
    Can1->SetGridy();
    Can1->ToggleEventStatus();


}
