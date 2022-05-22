#include "fits.c"

void peakRate_old()
{
        TTree *ids = (TTree*)gROOT->GetFile()->Get("ids");

	vector<double> peak = {117.0, 465.2, 1566.5};
	vector<double> rmin = {113.9, 459.9, 1562.6};
	vector<double> rmax = {120.1, 470.1, 1572.1};
	
	int nPeaks = peak.size();

	TCanvas *c = new TCanvas();
	c->Divide(1,nPeaks);

        TH1D *clov[nPeaks];

	for(int i=0; i<nPeaks; i++)
	{
		ostringstream oss, draw;
		oss << "clov[" << i << "]";
		draw << "E_Clov>>" << oss.str();

		double histMin = rmin[i] - 10;
		double histMax = rmax[i] + 10;
		int nBins = 5*( histMax - histMin );

		c->cd(i+1);

		clov[i] = new TH1D(oss.str().c_str(),oss.str().c_str(),nBins,histMin,histMax);

		ids->Draw(draw.str().c_str(),"M_Beta>0");

		cout << "\n\n   Peak: " << peak[i] << "\t rMin: " << rmin[i] << "\t rmax: " << rmax[i] << "\n";

		gamFit(oss.str().c_str(), rmin[i], rmax[i], 1, peak[i]);
	}
}
