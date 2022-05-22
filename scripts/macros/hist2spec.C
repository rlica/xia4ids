//Last edit: RL 22/05/2022
//RUN WITH: root -l -q hist2spec.C"(Run_no)"; 


void hist2spec(const int Run_no){

TFile *f = new TFile(Form("Run%03d.root", Run_no));
TH1F *h = (TH1F*)f->Get("h16_Si");

FILE *fp;
fp=fopen(Form("spec%d.txt", Run_no), "wt");
	
Int_t n = h->GetNbinsX();
   
   for (Int_t i=1; i<=n; i++) {
      fprintf(fp, "%d \t %d \n",
             int(h->GetBinLowEdge(i)+h->GetBinWidth(i)/2),
             int(h->GetBinContent(i)));
   }

cout << "Histogram written to " << Form("spec%d.txt", Run_no) << endl;
cout << "NBins: " << n << endl;   


}
