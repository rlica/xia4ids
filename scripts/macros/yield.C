//RUN WITH: root -l -q yield.C"(Run_no)"; 
//THIS is used for both GRAFANA output and RILIS scans.
//Modify only the gates (left, right)!




void yield(const int Run_no){
	
char E1[10]    = "E_Clov"; 
char E2[10]    = "E_Si";     
TCut E_LIMITS;
		
int left, right, AREA, BG;
float RATE, unit_time = 1; //  in seconds

	
FILE *fp;
fp=fopen(Form("out%d.txt", Run_no), "wt");


TChain ids("ids");
ids.Add( Form("Run%03d.root", Run_no) );
		
gStyle->SetOptStat(1000001);

//Reading the recording time
float Rec_time;

TLeaf *leaf_T = ids.GetLeaf("TIMESTAMP");
   leaf_T->GetBranch()->GetEntry(1);
   Rec_time = leaf_T->GetValue();
   leaf_T->GetBranch()->GetEntry(ids.GetEntries()-1);
   Rec_time = (leaf_T->GetValue() - Rec_time) * unit_time;
   
//---------------------------------------------------------------------------------------------
// Gamma - 1st gate

// 
// left = 727;
// right = 731;
// 
// //Reading the AREA
// E_LIMITS = Form("%s>%d && %s<%d", E1, left, E1, right);
// AREA = ids.GetEntries(E_LIMITS);
// 
// //Reading the BACKGROUND
// E_LIMITS = Form("%s>%d && %s<%d", E1, left - (right-left), E1, left );
// BG = ids.GetEntries(E_LIMITS);
// 
// E_LIMITS = Form("%s>%d && %s<%d", E1, right, E1, right + (right-left));
// BG += ids.GetEntries(E_LIMITS);
// 
// BG = BG/2;
// 
// RATE = (AREA - BG);
// 
// cout 	<< " Area = " << AREA  
// 	<< " BG = "   << BG 
// 	<< " Rate = " << RATE 
// 	<< endl;
// 
// // Write to output file
// 
// fprintf(fp,  "%d\t%.1f\t%.1f\t", Run_no, Rec_time, RATE);
//---------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------- 
// Si - 2nd gate
// no background

 left = 5000;
right = 6000;
BG=0;


//Reading the AREA
E_LIMITS = Form("%s>%d && %s<%d", E2, left, E2, right);
AREA = ids.GetEntries(E_LIMITS);

RATE = (AREA - BG);


cout	<< " Area = " << AREA  
	<< " BG = "   << BG 
	<< " Rate = " << RATE 
	<< endl;

// Write to output file

fprintf(fp,  "%d\t%.1f\t%.1f\t", Run_no, Rec_time, RATE); // Comment this line and uncomment the next if the Ge rates are enabled

//fprintf(fp, "%.2f\t", RATE);

//---------------------------------------------------------------------------------------------





}
