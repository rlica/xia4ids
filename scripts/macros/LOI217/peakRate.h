int nPeaks;
double binFactor;
double binF;
double rangeMin, rangeMax;
double conf_int = 0.683;

vector<double> myGamFit(const char* histo, double rMin, double rMax, int nOfPeaks, ...);
double fGam(double *x, double *par);
double   bg(double *x, double *par);
TH1D *getHisto(const char *histo);
