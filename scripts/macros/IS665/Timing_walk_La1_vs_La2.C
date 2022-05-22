

//Run with:
//   root -l Timing_walk_La1.C


void Timing_walk_La1_vs_La2(int run_number = 1){

  gROOT->Reset();
  gStyle->SetOptStat(1000000);
  gStyle->SetOptFit(11);

//___________________________________________
//Define the Run numbers
  char Run_name[20];
  sprintf(Run_name, "Run%03d.root", run_number);
  TChain ids("ids");
  ids.Add(Run_name);
  //ids.Add("Run047.root");

//___________________________________________  
//Define Detectors
  const char *E1    = "E_La[1]",   //Reference detector  
             *E2    = "E_La[0]",   //Draw walk curve for this detector
             *Tac   = "E_TAC_La1_La2";
//___________________________________________  
//Define Gates
  
  int   E1_gate_low  = 1760,
        E1_gate_high = 1860,
    
        E2_gate_low  = 2190,
        E2_gate_high = 2290;
      
  int    Tac_centroid,
         Tac_range   =  1000;

/*
//___________________________________________  
//Define Detectors
  const char *E1    = "E_LaBr1",
             *E2    = "E_LaBr2",
             *Tac   = "E_TAC_12";
//___________________________________________  
//Define Gates
  
  int   E1_gate_low  = 1450,
        E1_gate_high = 1540,
    
        E2_gate_low  = 1468,
        E2_gate_high = 1540;
      
  int    Tac_centroid,
         Tac_range   =  500;
*/
  double mean, err_mean, sigma, fwhm;
  
  //fwhm = 2.35482 * sigma 
  
  const char *gate    = Form("%s>%d && %s<%d && %s>%d && %s<%d", 
                          E1, E1_gate_low, E1, E1_gate_high, E2, E2_gate_low, E2, E2_gate_high);
 
 
  
  TH1I *hTemp      = new TH1I("hTemp","",16000,1,16000);
  TH1I *hGate     =  new TH1I("hGated", Form("%s  %s projection  [%s(%d,%d)] - Peak", Run_name, Tac, E2, E2_gate_low, E2_gate_high)   ,16000,1,16000);
 
 
  TCanvas *Can1;
  Can1 = new TCanvas("FastTiming","Fast Timing",800,800); 
  Can1->Divide(1,2);

  //gaus = A gaussian with 3 parameters: f(x) = p0*exp(-0.5*((x-p1)/p2)^2)
  //p1->GetParameter(0);//this is the constant
  //p1->GetParameter(1);//this is the mean
  //p1->GetParameter(2); //this is the sigma
  TF1 *g1    = new TF1("g1","gaus",10,16000);
  
  
  
  //Final
  TPad *p2 = (TPad *)(Can1->cd(2)); 
  
  ids.Draw(Form("%s>>hTemp", Tac),gate);
  hGate->Add(hTemp);
  
  hGate->Draw();
 
  //Fit
  hGate->Fit(g1,"WN");
  Tac_centroid = g1->GetParameter(1); 
  
  hGate->Fit(g1,"","",Tac_centroid-Tac_range, Tac_centroid+Tac_range);
  Tac_centroid = g1->GetParameter(1); 
    
  sigma = g1->GetParameter(2);
  fwhm = 2.35482 * sigma; 
  
  
  TText *label = new TText();
  label -> DrawText(Tac_centroid+100, 10, Form("FWHM = %.2f", fwhm));
  
  hGate->GetXaxis()->SetRangeUser(Tac_centroid-Tac_range, Tac_centroid+Tac_range);

  
  
  //2D
  
  const char *gate_E1 = Form("%s>%d && %s<%d", 
                          E1, E1_gate_low, E1, E1_gate_high);
  const char *plot_Tac_E2 = Form("%s:%s>>(500,1,%d,500,%d,%d)", 
                          Tac, E2, E2_gate_high+100, Tac_centroid-Tac_range, Tac_centroid+Tac_range);
  
  TPad *p1 = (TPad *)(Can1->cd(1));
  printf("Creating 2D plot of %s - %s with gate %s\n", Tac, E1, gate);
  //ids.Draw("E_TAC_12:E_LaBr2>>(250,1,2000,200,9000,11000)", "E_LaBr1 > 1440 && E_LaBr1 < 1540", "colz");

  ids.Draw(plot_Tac_E2, gate_E1, "colz");
  
  TLine *line = new TLine(2000,Tac_centroid,1,Tac_centroid);
  line->SetLineStyle(5);
  line->SetLineWidth(3);
  line->Draw();

  
    

  
  
}
