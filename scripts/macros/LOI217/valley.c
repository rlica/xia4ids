void valley()
{
	//vector<int> file = {293,295,296,298,299,300,301,303,304,305,306,307,308,309};
	vector<int> file = {310,311,312,314,315,316,317,318};

	double low[] = {995,1000};
	double hi[]  = {1000,1004};

	double val, peak, R, eR;

	TGraphErrors *g = new TGraphErrors(file.size());

	for(int i=0; i<file.size(); i++)
	{
		ostringstream fileName, gate1, gate2;
		fileName << "Run" << file[i] << ".root";
		TFile *f = new TFile(fileName.str().c_str());
		TTree *t = (TTree*)f->Get("ids");

		gate1 << "E_Clov>" << low[0] << "&&E_Clov<" << hi[0];
		gate2 << "E_Clov>" << low[1] << "&&E_Clov<" << hi[1];

		val  = t->GetEntries(gate1.str().c_str());
		peak = t->GetEntries(gate2.str().c_str());
		R  = val/peak;
		eR = R*sqrt( 1./val + 1./peak );
		cout << "\n    file: " << file[i] << "\t val: " << val << "\t peak: " << peak << "\t R: " << R << "\t eR: " << eR;
		g->SetPoint(i, file[i], R);
		g->SetPointError(i, 0, eR);
	}
	cout << "\n\n";
	g->Draw("AP");
}
