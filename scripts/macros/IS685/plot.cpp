{

TFile *f1 = new TFile("130Cd.root");
/*
TCanvas *c1 = new TCanvas("c1","c1");
c1->Divide(2,3);
c1->cd(0);
ids->Draw("E_TAC[3]>>h1(2000,0,60000)","(E_LaBr[0]>4000 && E_LaBr[0]<4400) && E_TAC[3]>0 && E_LaBr[1]==0","same");
c1->cd(1);
ids->Draw("E_TAC[3]>>h1(2000,0,60000)","(E_LaBr[0]>4000 && E_LaBr[0]<4400) && E_TAC[3]>0 && E_LaBr[1]==0","same");
c1->cd(2);
ids->Draw("E_TAC[3]>>h1(2000,0,60000)","(E_LaBr[0]>4000 && E_LaBr[0]<4400) && E_TAC[3]>0 && E_LaBr[1]==0","same");
c1->cd(3);
ids->Draw("E_TAC[3]>>h1(2000,0,60000)","(E_LaBr[0]>4000 && E_LaBr[0]<4400) && E_TAC[3]>0 && E_LaBr[1]==0","same");
c1->cd(4);
ids->Draw("E_TAC[3]>>h1(2000,0,60000)","(E_LaBr[0]>4000 && E_LaBr[0]<4400) && E_TAC[3]>0 && E_LaBr[1]==0","same");
c1->cd(5);
ids->Draw("E_TAC[3]>>h1(2000,0,60000)","(E_LaBr[0]>4000 && E_LaBr[0]<4400) && E_TAC[3]>0 && E_LaBr[1]==0","same");
c1->cd(6);
ids->Draw("E_TAC[3]>>h1(2000,0,60000)","(E_LaBr[0]>4000 && E_LaBr[0]<4400) && E_TAC[3]>0 && E_LaBr[1]==0","same");


c1->SaveAs("TAC3_130Cd.pdf");
*/

TCanvas *c1 = new TCanvas("c1","c1");
ids->Draw("E_Clov[]>>h1(20000,0,10000)");
//c1->SaveAs("TAC3_130Cd.pdf");


}
