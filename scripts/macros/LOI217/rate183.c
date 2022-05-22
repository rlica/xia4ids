#include "fits.c"

void rate183()
{
	gROOT->ProcessLine(".x draw_clov.c");

	gamFit("clov",353.,359.,1,356.4);
	gamFit("clov",265.5,278.,3,268.5,272.4,276.);

	TH1D *clov = (TH1D*)gROOT->GetFile()->Get("clov");
	clov->GetXaxis()->SetRangeUser(267,364);
}
