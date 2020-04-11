//usr/bin/env -S root.exe "${0}(\"${1}\")" ; exit $?

void histo_storage_files(const char* input) {

TString file(input);

const Double_t bin_list [] = {0., 4*1024, 16*1024, 32.*1024, 64.*1024, 512.*1024, 1.*1048576, 10.*1048576, 100.*1048576, 500.*1048576, 1.*1073741824, 3.*1073741824};
const char* labels[] = {"<4K", "<16K", "<32K", "<64K", "<512K", "<1MB", "<10MB", "<100MB", "<500MB", "<1GB", "<3GB", ">3GB" };
const int bins = sizeof(bin_list)/sizeof(bin_list[0]) - 1;

//#####################################################################
gStyle->SetPalette(kVisibleSpectrum);
Int_t MyPalette[bins+1];
const Int_t Number = 3;
Double_t Red[Number]    = { 1.00, 0.00, 0.00};
Double_t Green[Number]  = { 0.00, 1.00, 0.00};
Double_t Blue[Number]   = { 1.00, 0.00, 1.00};
Double_t Stops[Number]  = { 0.00, 0.50, 1.00 };
Int_t FI = TColor::CreateGradientColorTable(3, Stops, Red, Green, Blue, bins);
for (int i=0;i<bins;i++) { MyPalette[i] = FI+i; }

//#####################################################################
TCanvas* c1 = new TCanvas("fsizes","File sizes distribution", 1480, 1024);
c1->cd();

TH1I* h1 = new TH1I("fsizes","File sizes distribution;size(bytes);Nr", bins, bin_list);
TTree* T = new TTree("ntuple","data from ascii file");
Long64_t nlines = T->ReadFile(file.Data(),"x");
std::cout << "Nr files : " << nlines << std::endl;
T->Draw("x >> fsizes");

Double_t values [bins + 1] = {0};
for ( int i = 1; i <= bins + 1; i++ ) {
    values [i] = h1->GetBinContent(i);
    }

Bool_t do_pie = kTRUE; //do_pie = kFALSE;

if (do_pie) {
    TPie *pie = new TPie("pie", "File sizes distribution",bins,values,MyPalette,labels);
    pie->SetRadius(.2);
    pie->SetLabelsOffset(.02);
    pie->SetLabelFormat("%perc %txt");
    pie->SetFractionFormat("%4.2f");
    //pie->SetAngularOffset(45);
    pie->Draw();
    //pie->MakeLegend();
    }
else {
    gStyle->SetOptStat(11111111);
    h1->Draw();
    }

}

