#include "peakRate.h"

void luxuryPeakRate2()
{

	//209Tl:
	// Peaks and ranges for fits
	//vector<double> peak = {117.1, 465.2, 1566.5};
	//vector<double> rmin = {113.9, 459.9, 1562.6};
	//vector<double> rmax = {120.1, 470.1, 1572.1};
	
	//209Fr:
	// Peaks and ranges for fits
	vector<double> peak = {798, 547, 653, 376};
	vector<double> rmin = {798-5, 547-5, 653-5, 376-5};
	vector<double> rmax = {798+5, 547+5, 653+5, 376+5};
	
	//211Fr:
	// Peaks and ranges for fits
	//vector<double> peak = {540, 918, 281};
	//vector<double> rmin = {540-5, 918-5, 281-5};
	//vector<double> rmax = {540+5, 918+5, 281+5};
	
	//210Tl:
	// Peaks and ranges for fits
	//vector<double> peak = {799, 296};
	//vector<double> rmin = {799-5, 296-5};
	//vector<double> rmax = {799+5, 799+5};

	//208Tl
	// Peaks and ranges for fits
	//vector<double> peak = {2615,583};
	//vector<double> rmin = {2605, 580};
	//vector<double> rmax = {2620,586};

	//211Tl:
	// Peaks and ranges for fits
	//vector<double> peak = {316.1, 362.1, 597.1, 1190.1};
	//vector<double> rmin = {312.9, 361.1, 593.9, 1184.9};
	//vector<double> rmax = {320.1, 365., 598.0, 1195.1};

	// Get tree
        TTree *ids = (TTree*)gROOT->GetFile()->Get("ids");

	// Get run time for file
	vector<double> timeStamp;
	ULong64_t tStamp;	
	ids->SetBranchAddress("Timestamp",&tStamp);
	ids->GetEntry(1);
	timeStamp.push_back(tStamp);
	ids->GetEntry(ids->GetEntries()-1);	
	timeStamp.push_back(tStamp);
	double time = timeStamp[1] - timeStamp[0];

	// Loop over peaks and fit, store integrals,
	// uncertainties, and sums for peaks
	int nPeaks = peak.size();

	double sumCounts=0;
	double sumErrSquared=0;

	TCanvas *c = new TCanvas();
	c->Divide(1,nPeaks);

        TH1D *clov[nPeaks];

	vector< vector<double> > fitResult;

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

		//ids->Draw(draw.str().c_str(),"M_Beta>0");
		ids->Draw(draw.str().c_str(),"");

		fitResult.push_back( myGamFit(oss.str().c_str(), rmin[i], rmax[i], 1, peak[i]));

		sumCounts     += fitResult[i][0];
		sumErrSquared += pow(fitResult[i][1],2);
	}

	// Output results to terminal
	cout << "\n\n\n";
	cout << "   Run time: " << time << " s\n";
	cout << "   Energy \t Counts \t err C \t\t Rate \t\t err R \n";
	for(int i=0; i<nPeaks; i++) cout << "   " << peak[i] << " \t " << fitResult[i][0] << " \t " << fitResult[i][1] << " \t " << 1.0*fitResult[i][0]/time << " \t " << 1.0*fitResult[i][1]/time  << "\n";
	cout << "   ALL  \t " << sumCounts << " \t " << pow(sumErrSquared,0.5) << " \t " << sumCounts/time << " \t " << 1.0*pow(sumErrSquared,0.5)/time << "\n\n\n";
}

vector<double> myGamFit(const char* histo, double rMin, double rMax, int nOfPeaks, ...)
{
        TH1D *h = getHisto(histo);
        h->Draw("hist");

        double maxArea = h->GetEntries();

        double lim_Area[2]  = {10, maxArea};            // Area
        double E_tol        = 1.5;                      // +/- E est
        double lim_sigma[3] = {0.5, 0.35, 1.5};         // sigma

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


        vector<double> ePeak, aPeak;
        va_list vl;
        va_start(vl,nOfPeaks);

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

	vector<double> resultArea = {f1->GetParameter(4),f1->GetParError(4)};
	return resultArea;
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
