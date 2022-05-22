

void Clover_singles(){

gStyle->SetOptStat(0000000);



// Draw some plots	

TCanvas *Can1;
Can1 = new TCanvas("Can1","Can1",800,800);
Can1->SetLogy();
Can1->Divide(2,2);

TPad *p1 = (TPad *)(Can1->cd(1)); 
//p1->SetLogy();

h0_Clov1->SetLineColor(1);
h1_Clov1->SetLineColor(2);
h2_Clov1->SetLineColor(3);
h3_Clov1->SetLineColor(4);

h0_Clov1->Draw();
h1_Clov1->Draw("same");
h2_Clov1->Draw("same");
h3_Clov1->Draw("same");


TPad *p2 = (TPad *)(Can1->cd(2)); 

h4_Clov2->SetLineColor(1);
h5_Clov2->SetLineColor(2);
h6_Clov2->SetLineColor(3);
h7_Clov2->SetLineColor(4);

h4_Clov2->Draw();
h5_Clov2->Draw("same");
h6_Clov2->Draw("same");
h7_Clov2->Draw("same");

TPad *p3 = (TPad *)(Can1->cd(3)); 

h8_Clov3->SetLineColor(1);
h9_Clov3->SetLineColor(2);
h10_Clov3->SetLineColor(3);
h11_Clov3->SetLineColor(4);

h8_Clov3->Draw();
h9_Clov3->Draw("same");
h10_Clov3->Draw("same");
h11_Clov3->Draw("same");


TPad *p4 = (TPad *)(Can1->cd(4)); 

h12_Clov4->SetLineColor(1);
h13_Clov4->SetLineColor(2);
h14_Clov4->SetLineColor(3);
h15_Clov4->SetLineColor(4);

h12_Clov4->Draw();
h13_Clov4->Draw("same");
h14_Clov4->Draw("same");
h15_Clov4->Draw("same");


}
