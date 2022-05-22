void beta_gam()
{
	TTree *ids = (TTree*)gROOT->GetFile()->Get("ids");
	TH1D *clov = new TH1D("clov","clov",25000,0,5000);

	ids->Draw("E_Clov>>clov","E_Clov>5 && M_Beta>0");
}
