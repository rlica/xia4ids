#include <iostream>
#include <sstream>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include "TFitResult.h"
#include "Math/PdfFuncMathCore.h"
#include "TVirtualFitter.h"
#include "TROOT.h"

#include "fits.h"

void gamFit(const char* histo, double rMin, double rMax, int nOfPeaks, ...)
{
	TH1D *h = getHisto(histo);
	h->Draw("hist");

	double maxArea = h->GetEntries();

	double lim_Area[2]  = {10, maxArea};		// Area
	double E_tol        = 1.5;			// +/- E est
	double lim_sigma[3] = {0.5, 0.35, 1.5};		// sigma

	rangeMin = rMin;
	rangeMax = rMax;

	int x1 = h->GetBin(rangeMin);
	int x2 = h->GetBin(rangeMax);
	double y1 = h->GetBinContent(x1);
	double y2 = h->GetBinContent(x2);

	binFactor = h->GetBinWidth(0);

	double grad = (y2-y1)/(x2-x1);
	double offs = y2 - grad*x2;

	grad = 0.0;
	offs = 5.;

	TF1 *f1 = new TF1("f1",fGam,rangeMin,rangeMax,3+2*nOfPeaks);
	TF1 *f2 = new TF1("f2",bg,rangeMin,rangeMax,2);
	f1->SetNpx(1000);

	f1->SetParameter(0,offs);
	f1->SetParameter(1,grad);
	f1->SetParameter(2,lim_sigma[0]);
	f1->SetParLimits(2,lim_sigma[1],lim_sigma[2]);

	f1->SetParName(0,"BG offset");
	f1->SetParName(1,"BG grad");
	f1->SetParName(2,"sigma");

	// Vectors for storing centroid & height estimates
	vector<double> ePeak, aPeak;

	// Variable list for reading in your N variables
	va_list vl;
	va_start(vl,nOfPeaks);

	// Start reading in your variables, find the min and max energy value
	// and set your histo+fit limits to min-100,max+100
	double val;
	cout << "\n";

	for(int i=0; i<nOfPeaks; i++)
	{
		val = 1.0*va_arg(vl, double);
		ePeak.push_back(val);
	}

	nPeaks = ePeak.size();

	for(int i=0; i<nOfPeaks;i++)
	{
		ostringstream eName,aName;
		eName << i+1 << ". Energy";
		aName << i+1 << ". Area";

		f1->SetParName(2*i+3,eName.str().c_str());
		f1->SetParameter(2*i+3,ePeak[i]);
		f1->SetParLimits(2*i+3,ePeak[i]-E_tol,ePeak[i]+E_tol);

		f1->SetParName(2*i+4,aName.str().c_str());
		f1->SetParameter(2*i+4,100);
		f1->SetParLimits(2*i+4,lim_Area[0],lim_Area[1]);
	}

	h->Fit(f1,"E M R+");
	f1->Draw("same");

	f2->FixParameter(0,f1->GetParameter(0));
	f2->FixParameter(1,f1->GetParameter(1));
	f2->SetLineColor(4);
	
	TF1 *ip[nOfPeaks];
	for(int i=0; i<nOfPeaks; i++)
	{
		ostringstream ipT;
		ipT << "ip[" << i << "]";
		ip[i] = new TF1(ipT.str().c_str(),fGam,rangeMin,rangeMax,5);
		ip[i]->FixParameter(0,f1->GetParameter(0));
		ip[i]->FixParameter(1,f1->GetParameter(1));
		ip[i]->FixParameter(2,f1->GetParameter(2));
		ip[i]->FixParameter(3,f1->GetParameter(2*i+3));
		ip[i]->FixParameter(4,f1->GetParameter(2*i+4));
		ip[i]->SetLineColor(4);
		ip[i]->SetLineStyle(2);
		ip[i]->SetLineWidth(2);
		ip[i]->SetNpx(1000);
		ip[i]->Draw("same");
	}
	f2->Draw("same");
}
/********************************************************
*                 BG + gammas function                  *
********************************************************/
// Fit for multiple peaks
double fGam(double *x, double *par)
{
	double xcur  = x[0];
 
	double y0  = par[0]; 
	double grd = par[1]; 
	double sig = par[2];
	
	double sum = y0 + grd*xcur;

	for (int i=0; i<nPeaks; i++)
	{
		double C = par[3+2*i];
		double A = binFactor*par[4+2*i];
		sum += A*TMath::Gaus(xcur, C, sig, kTRUE);
	}
	return sum;
}

double bg(double *x, double *par)
{
	return par[0] + par[1]*x[0];
}

TH1D *getHisto(const char *histo)
{
	return (TH1D*)gROOT->GetFile()->Get(histo);
}
