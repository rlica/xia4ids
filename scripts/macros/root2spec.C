
//RUN WITH: root -l -q root2spec.C"(Run_no)"; 


void root2spec(const int Run_no){

const char *E    = "E_Clov"; 
const int SIZE = 8192;
int spec[SIZE];
int overflow=0, underflow=0, entries=0;

FILE *fp;
fp=fopen(Form("spec%d.a%d", Run_no, SIZE/1000), "wt");

TChain t("ids");
t.Add( Form("Run%03d.root", Run_no) );

TLeaf *leaf = t.GetLeaf(E);
				


for (int i = 0; i<SIZE; i++) {
	spec[i] = 0;
}

for (int i = 0; i < t.GetEntries(); i++) {
	leaf->GetBranch()->GetEntry(i);
	int value = leaf->GetValue();
	if (value > 0) {
		if  (value < SIZE) {
			spec[value]++;
			entries++;
		}
		else overflow++;
	}
	else underflow++;
}


for (int i = 0; i < SIZE; i++) {
	fprintf(fp, "%d\t%d\n", i, spec[i]);
}

cout << "Data written to " << Form("spec%d.a%d", Run_no, SIZE/1000) << endl;
cout << "Entries: " << entries << " Overflow: " << overflow << " Underflow: " << underflow << endl;   



}
