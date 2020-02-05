#pragma once

#if !defined(__CINT__) || defined(__MAKECINT__) || defined(__CLING__) || defined(__ROOTCLING__)

#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <cstring>

#include <vector>
#include <algorithm>
#include <utility>
#include <cstdio>

#include <Rtypes.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TProfile.h>
#include <TH1.h>
#include <TH2.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TProfile.h>
#include <TMath.h>
#include <TLorentzVector.h>
#include <TVector2.h>
#include <TClonesArray.h>
#include <THnSparse.h>
#include <TSeqCollection.h>
#include <TCollection.h>
#include <TList.h>
#include <THashList.h>
#include <TFile.h>
#include <TAttMarker.h>
#include <TLegend.h>
#include <TLegendEntry.h>
#include <TAttMarker.h>
#include <TStyle.h>
#include <TPad.h>
#include <TVirtualPad.h>
#include <TCanvas.h>
#include <TSystemDirectory.h>
#include <TInterpreter.h>

#include <TRatioPlot.h>
#include <TString.h>
#include <TObjString.h>
#include <AliEmcalList.h>

#endif

using namespace std;

// Preferred colors and markers
const Int_t fillColors[] = {kGray+1,  kRed-10, kBlue-9, kGreen-8, kMagenta-9, kOrange-9,kCyan-8,kYellow-7}; // for syst bands
const Int_t colors[]     = {kBlack, kRed+1 , kBlue+1, kGreen+3, kMagenta+1, kOrange-1,kCyan+2,kYellow+2};
const Int_t markers[]    = {kFullCircle, kOpenCircle, kFullSquare, kOpenSquare, kFullCross, kOpenCross, kFullDiamond, kOpenDiamond, kFullStar, kOpenStar, kFullTriangleUp, kOpenTriangleUp, kFullTriangleDown, kOpenTriangleDown};

//void ROOT_FUNCTIONS(){ }

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

std::pair <TString, TString> HistoNameTitle (const TString& base, const TString& radius, const TString& type, const TString& range, const TString& title_suffix) {
  TString name = base + radius + type + range;
  TString range_mod = range;
  TString title = range_mod.Replace(0,1,"").ReplaceAll("_","-") + title_suffix;
  return std::make_pair(name, title);
}

//______________________________________________________________
void print_canvas (TPad* c, TString name) {
  name += ".png";
  c->Print(name.Data());
}

//______________________________________________________________
TH1D* getH1dir (TFile& f, const TString dir , const TString hname) {
  TString path = dir + "/" + hname;
  return dynamic_cast<TH1D*>(f.Get(path.Data()));
  }

//______________________________________________________________
TH1D* getH1 (THashList* list, const char* hname, Int_t cent = 0) {
  if (!list) { std::cout << "invalid list" << endl; return NULL;}
  return dynamic_cast<TH1D*>( list->FindObject( Form("%s_%i",hname,cent)) );
  }

//______________________________________________________________
TH2D* getH2 (THashList* list, const char* hname, Int_t cent = 0) {
  if (!list) { std::cout << "invalid list" << endl; return NULL;}
  return dynamic_cast<TH2D*>( list->FindObject( Form("%s_%i",hname,cent)) );
}

//______________________________________________________________
void val2bin (unsigned int val) {
for ( unsigned short i = 0; i <= 31; ++i) { std::cout << ((val >> i) & 1); };
cout << endl;
}
//______________________________________________________________
char* ObjGetPtr ( void* ptr /*or if on stack use &obj */) {
  if (!ptr) {return NULL;}
  std::stringstream obj_str;
  obj_str << ptr; // record the pointer
  return Form( "%s", obj_str.str().c_str() );
}

//______________________________________________________________
const char* ObjGetName (TObject* ptr) {
  return reinterpret_cast<TObject*>(ptr)->ClassName();
  }

//______________________________________________________________
TObject* Apply2Ptr (TObject* obj, const char* arg) {
  // apply the method (arg) to the object
  TInterpreter* my_int = TInterpreter::Instance();
  my_int->ProcessLine(Form( "(( reinterpret_cast<%s*>(%s) )->%s);" , ObjGetName(obj) , ObjGetPtr(obj), arg ));
  return obj;
}

//______________________________________________________________
void ForEachH (const char* method, const char* params) {
  if ( gROOT->GetListOfCanvases()->GetEntries() == 0 ) {return;}

  TSeqCollection* list_canvases = gROOT->GetListOfCanvases();
  for (int i=0; i<list_canvases->GetEntries(); ++i) {
    TPad* canvas = dynamic_cast<TPad*> (list_canvases->At(i));
    for(const auto&& obj: *(canvas->GetListOfPrimitives())) {
      if ( obj->InheritsFrom("TH1") ) { obj->Execute(method, params); canvas->Modified(); canvas->Update(); }

      if ( obj->InheritsFrom("TPad") ) {
        TPad* pad = dynamic_cast<TPad*>(obj);
        TList* padlist = pad->GetListOfPrimitives();
        for (int i=0; i<padlist->GetEntries(); ++i) {
          TObject* h_in_padlist = padlist->At(i);
          if ( h_in_padlist->InheritsFrom("TH1") ) { h_in_padlist->Execute(method, params); pad->Modified(); pad->Update(); }
          }
        }
      }
    }
  }

//______________________________________________________________
void SaveAllHistos (const char* file_name, const char* directory) {
    if ( gROOT->GetListOfCanvases()->GetEntries() == 0 ) {return;}
    TFile* results_file = new TFile(file_name, "UPDATE");
    if (results_file->IsZombie()) return;
    if (!results_file->GetDirectory(directory)) { results_file->mkdir(directory); }
    results_file->cd(directory);

    TSeqCollection* list_canvases = gROOT->GetListOfCanvases();
    for (int i=0; i<list_canvases->GetEntries(); ++i) {
      TPad* canvas = dynamic_cast<TPad*> (list_canvases->At(i));
      for(const auto&& obj: *(canvas->GetListOfPrimitives())) {
        if ( obj->InheritsFrom("TH1") ) { obj->Write(NULL, TObject::kOverwrite); }
        if ( obj->InheritsFrom("TPad") ) {
          TPad* pad = dynamic_cast<TPad*>(obj);
          TList* padlist = pad->GetListOfPrimitives();
          for (int i=0; i<padlist->GetEntries(); ++i) {
            TObject* h_in_padlist = padlist->At(i);
            if ( h_in_padlist->InheritsFrom("TH1") ) { h_in_padlist->Write(NULL, TObject::kOverwrite); }
          }
        }
      }
    }
  }

//______________________________________________________________
TH1D* ProjectY (TH2D* h, Option_t* option = "") {
  TString new_name = h->GetName();
  new_name += "_projY";
  return h->ProjectionY(new_name.Data(), 0, -1, option );
  }

//______________________________________________________________
TH1D* ProjectX (TH2D* h, Option_t* option = "") {
  TString new_name = h->GetName();
  new_name += "_projX";
  return h->ProjectionX(new_name.Data(), 0, -1, option );
}

//______________________________________________________________
void PadToLogScale() {
  if ( gROOT->GetListOfCanvases()->GetEntries() == 0 ) return;
  if ( gPad->GetLogy() ) {return;}

  TPad* oldpad = dynamic_cast<TPad*> gPad;
  // Now loop through histos and find the extrema
  for(const auto&& obj: *(oldpad->GetListOfPrimitives())) {
    if ( obj->InheritsFrom("TH1") ) {
      // If there is a 2D histogram, don't do anything.
      if ( obj->InheritsFrom("TH2") ) { return; }
      (dynamic_cast<TH1*>(obj))->SetMinimum();
      }
    }
  gPad->SetLogy();
  gPad->Modified();
}

//______________________________________________________________
TPad* PadToLogScale(TPad* pad) {
  if ( gROOT->GetListOfCanvases()->GetEntries() == 0 ) return NULL;
  if ( pad->GetLogy() ) {return NULL;}
  
  // Now loop through histos and find the extrema
  for(const auto&& obj: *(pad->GetListOfPrimitives())) {
    if ( obj->InheritsFrom("TH1") ) {
      // If there is a 2D histogram, don't do anything.
      if ( obj->InheritsFrom("TH2") ) { return NULL; }
      (dynamic_cast<TH1*>(obj))->SetMinimum();
      }
    }
  pad->SetLogy();
  pad->Modified();
  return pad;
  }

//______________________________________________________________
void PadToLogScale_AutoScale() {
  if ( gROOT->GetListOfCanvases()->GetEntries() == 0 ) return;
  if ( gPad->GetLogy() ) {return;}

  TPad* oldpad = dynamic_cast<TPad*> gPad;
  // Now loop through histos and find the extrema
  TList* glist = oldpad->GetListOfPrimitives();
  for (int i=0; i<glist->GetEntries(); ++i) {
    TObject *obj = glist->At(i);
    if ( obj->InheritsFrom("TH1") ) {
      // If there is a 2D histogram, don't do anything.
      if ( obj->InheritsFrom("TH2") ) { return; }
      (dynamic_cast<TH1*>(obj))->SetMinimum();
      Double_t min=0, max=999999999;
      min = (dynamic_cast<TH1*>(obj))->GetMinimum();
      max = (dynamic_cast<TH1*>(obj))->GetMaximum();
      (dynamic_cast<TH1*>(obj))->GetYaxis()->SetRangeUser(min, max);
      }
    }
  gPad->SetLogy();
  gPad->Modified();
}

//______________________________________________________________
void PadRebin(Int_t binsgroup) {
  if ( gROOT->GetListOfCanvases()->GetEntries() == 0 ) return;

  TPad* oldpad = dynamic_cast<TPad*> gPad;
  // Now loop through histos and find the extrema
  TList* glist = oldpad->GetListOfPrimitives();
  for (int i=0; i<glist->GetEntries(); ++i) {
    TObject *obj = glist->At(i);
    if ( obj->InheritsFrom("TH1") ) { (dynamic_cast<TH1*>(obj))->Rebin(binsgroup); }
    }
  gPad->Modified();
}

//______________________________________________________________
void PadSetYaxis(Double_t r1, Double_t r2) {
  if ( gROOT->GetListOfCanvases()->GetEntries() == 0 ) return;

  TPad* oldpad = dynamic_cast<TPad*> gPad;
  // Now loop through histos and find the extrema
  TList* glist = oldpad->GetListOfPrimitives();
  for (int i=0; i<glist->GetEntries(); ++i) {
    TObject *obj = glist->At(i);
    if ( obj->InheritsFrom("TH1") ) { (dynamic_cast<TH1*>(obj))->GetYaxis()->SetRangeUser(r1, r2); }
    }
  gPad->Modified();
}

//______________________________________________________________
void PadSetXaxis(Double_t r1, Double_t r2) {
  if ( gROOT->GetListOfCanvases()->GetEntries() == 0 ) return;

  TPad* oldpad = dynamic_cast<TPad*> gPad;
  // Now loop through histos and find the extrema
  TList* glist = oldpad->GetListOfPrimitives();
  for (int i=0; i<glist->GetEntries(); ++i) {
    TObject *obj = glist->At(i);
    if ( obj->InheritsFrom("TH1") ) { (dynamic_cast<TH1*>(obj))->GetXaxis()->SetRangeUser(r1, r2); }
    }
  gPad->Modified();
}

//______________________________________________________________
TPad* PadSetYaxis(TPad* pad, Double_t r1, Double_t r2) {
  if ( gROOT->GetListOfCanvases()->GetEntries() == 0 ) return NULL;
  // Now loop through histos and find the extrema
  TList* glist = pad->GetListOfPrimitives();
  for (int i=0; i<glist->GetEntries(); ++i) {
    TObject *obj = glist->At(i);
    if ( obj->InheritsFrom("TH1") ) { (dynamic_cast<TH1*>(obj))->GetYaxis()->SetRangeUser(r1, r2); }
    }
  pad->Modified();
  return pad;
  }

//______________________________________________________________
TPad* PadSetXaxis(TPad* pad, Double_t r1, Double_t r2) {
  if ( gROOT->GetListOfCanvases()->GetEntries() == 0 ) return NULL;
  // Now loop through histos and find the extrema
  TList* glist = pad->GetListOfPrimitives();
  for (int i=0; i<glist->GetEntries(); ++i) {
    TObject *obj = glist->At(i);
    if ( obj->InheritsFrom("TH1") ) { (dynamic_cast<TH1*>(obj))->GetXaxis()->SetRangeUser(r1, r2); }
    }
  pad->Modified();
  return pad;
  }
  
//______________________________________________________________
TH1D* Rebin (TH1D* h, Int_t nbins, Double_t* xbins) {
  return dynamic_cast<TH1D*>( h->Rebin(nbins, Form("%s_rebin",h->GetName()), xbins) );
  }

//______________________________________________________________
TH1D* Clone (TH1D* h) {
return dynamic_cast<TH1D*>(h->Clone( Form("%s_cl",h->GetName()) ));
  }

//______________________________________________________________
TH1* NormH ( TH1* h, Option_t* option = "") {
  if (!h) { cout << "invalid pointer" << endl; return NULL; }
  TH1::SetDefaultSumw2(kTRUE);

  if ( h->GetSumOfWeights() == 0) {
    Error("DrawNormalized","Sum of weights is null. Cannot normalize histogram: %s",h->GetName());
    return h;
    }

  h->Scale(1./h->GetEntries(), option);
  return h;
}

//______________________________________________________________
TH1* NormHn ( TH1* h, Double_t norm = 1., Option_t* option = "") {
  if (!h) { cout << "invalid pointer" << endl; return NULL; }
  TH1::SetDefaultSumw2(kTRUE);

  if ( h->GetSumOfWeights() == 0) {
    Error("DrawNormalized","Sum of weights is null. Cannot normalize histogram: %s",h->GetName());
    return NULL;
    }

  h->Scale(1./norm, option);
  return h;
}

//______________________________________________________________
TH1* ScaleH ( TH1* h, Double_t factor = 1., Option_t* option = "") {
  if (!h) { cout << "invalid pointer" << endl; return NULL; }
  TH1::SetDefaultSumw2(kTRUE);

  if ( h->GetSumOfWeights() == 0) {
    Error("DrawNormalized","Sum of weights is null. Cannot normalize histogram: %s",h->GetName());
    return NULL;
    }

  h->Scale(factor, option);
  return h;
}

//______________________________________________________________
TH1* SetAxis ( TH1* h, const char* axis_name, Float_t l_offset /*Label offset*/, Float_t l_size /*Label size*/, Float_t t_offset /*Title offset*/, Float_t t_size /*Title size*/) {
  if (!h) { cout << "invalid pointer" << endl; return NULL; }
  TAxis* axis = NULL;

  if ( (std::strcmp(axis_name, "x") == 0) || (std::strcmp(axis_name, "X") == 0) )
    { axis = h->GetXaxis();}
  else if ( (std::strcmp(axis_name, "y") == 0) || (std::strcmp(axis_name, "Y") == 0) )
    { axis = h->GetYaxis();}
  else
    return NULL;

  axis->SetLabelOffset(l_offset);
  axis->SetLabelSize(l_size);
  axis->SetTitleOffset(t_offset);
  axis->SetTitleSize(t_size);
  return h;
}


//______________________________________________________________
TH1* SetXProperties ( TH1* h, Float_t l_offset /*Label offset*/, Float_t l_size /*Label size*/, Float_t t_offset /*Title offset*/, Float_t t_size /*Title size*/) {
  return SetAxis (h, "x", l_offset, l_size, t_offset, t_size);
}

//______________________________________________________________
TH1* SetYProperties ( TH1* h, Float_t l_offset /*Label offset*/, Float_t l_size /*Label size*/, Float_t t_offset /*Title offset*/, Float_t t_size /*Title size*/) {
  return SetAxis (h, "y", l_offset, l_size, t_offset, t_size);
}

//______________________________________________________________
TH1* SetXRange ( TH1* h, Double_t r1 , Double_t r2) {
  if (!h) {cout << "invalid pointer" << endl; return NULL;}
  h->GetXaxis()->SetRangeUser(r1, r2);
  return h;
}

//______________________________________________________________
TH1* SetYRange ( TH1* h, Double_t r1 , Double_t r2) {
  if (!h) {cout << "invalid pointer" << endl; return NULL;}
  h->GetYaxis()->SetRangeUser(r1, r2);
  return h;
}

//______________________________________________________________
TH1* SetXYRange ( TH1* h, Double_t x_r1 , Double_t x_r2, Double_t y_r1 , Double_t y_r2) {
  if (!h) {cout << "invalid pointer" << endl; return NULL;}
  h->GetXaxis()->SetRangeUser(x_r1, x_r2);
  h->GetYaxis()->SetRangeUser(y_r1, y_r2);
  return h;
}

//______________________________________________________________
TH1* StyleColor ( TH1* h, Style_t mystyle = 1, Color_t mycolor = 1, Size_t mysize = 1.3) {
  if (!h) {cout << "invalid pointer" << endl; return NULL;}
  h->SetMarkerStyle(mystyle);
  h->SetMarkerColor(mycolor);
  h->SetMarkerSize(mysize);
  return h;
}

//______________________________________________________________
TH1* hDraw ( TH1* h, TString arg = "PE SAME" ) {
  if (!h) {cout << "invalid pointer" << endl; return NULL;}
  TString default_args ("PE SAME");

  if ( !arg.EqualTo(default_args) ) { arg += " " + default_args; }

  h->Draw(arg.Data());
  return h;
}

//___________________________________________________
void SetCanvasSize (TCanvas& c, UInt_t w, UInt_t h) {
if (gROOT->IsBatch())
    { c.SetWindowSize(w,h);}
else
    { c.SetWindowSize(w + (w - c.GetWw()), h + (h - c.GetWh())); }

    c.Modified();
    c.ForceUpdate();
}

//___________________________________________________
TCanvas* SetCanvasSize (TCanvas* c, UInt_t w, UInt_t h) {
  SetCanvasSize ( (*c), w, h);
return c;
}


//______________________________________________________________
TH1D* AddNewH (TH1D* h_old, TH1D* add_h, Double_t c1 = 1, TString prefix = "") {
    if (!h_old) {return NULL;}
    if (!add_h) {return NULL;}

    TString name = h_old->GetName();
    if (!prefix.IsNull()) { name = name + "_" + prefix; }

    h_old->Sumw2(kTRUE);
    TH1D* h_new = dynamic_cast<TH1D*> (h_old->Clone(name.Data()));
    h_new->Sumw2(kTRUE);

    h_new->Add(add_h, c1);

    return h_new;
}

//______________________________________________________________
TH2D* AddNewH (TH2D* h_old, TH2D* add_h, Double_t c1 = 1, TString prefix = "") {
    if (!h_old) {return NULL;}
    if (!add_h) {return NULL;}

    TString name = h_old->GetName();
    if (!prefix.IsNull()) { name = name + "_" + prefix; }

    h_old->Sumw2(kTRUE);
    TH2D* h_new = dynamic_cast<TH2D*> (h_old->Clone(name.Data()));
    h_new->Sumw2(kTRUE);

    h_new->Add(add_h, c1);

    return h_new;
}

//______________________________________________________________
Double_t Integral (const TH1D* h, Double_t val1, Double_t val2, Option_t* option = "" ) {
    if (!h) {return -99999;}

    Int_t bin1 = h->GetXaxis()->FindBin(val1);
    Int_t bin2 = h->GetXaxis()->FindBin(val2);

    Double_t integral = h->Integral( bin1, bin2, option );

    return integral;
}

//______________________________________________________________________________
void ls( TString path = ".") {
  TString s ("ls ");
  s += path;
  gSystem->Exec(s.Data());
}

//______________________________________________________________________________
void ll( TString path = ".") {
  TString s ("ls -la ");
  s += path;
  gSystem->Exec(s.Data());
}

//______________________________________________________________________________
const char* pwd() { return gSystem->WorkingDirectory(); }

//______________________________________________________________________________
const char* curdir() { return gSystem->BaseName( pwd() ); }

//______________________________________________________________________________
const char* cd ( TString path = ".") {
 if ( gSystem->ChangeDirectory(path.Data()) )
    { return pwd(); }
 else
    { printf ("Could not change dir"); return pwd(); }
}

//______________________________________________________________________________
Bool_t IsTreeType ( const TString& fileInput, const TString& treeName) {
  TFile* f = TFile::Open (fileInput.Data());

  if ( f->IsZombie() ) {
    std::cout << " :: Skipping un-openable file: << " << fileInput.Data()  << std::endl; return kFALSE;
    }
  else {
    TKey* key = f->FindKeyAny (treeName.Data());
    if (key) { f->Close(); return kTRUE;  }
    else     { f->Close(); return kFALSE; }
    }
}

//______________________________________________________________________________
TString FindTreeName ( const TString& file_list ) {
  TString fDataType = "";  // result
  if ( gSystem->AccessPathName ( file_list.Data() ) ) {
    std::cout << "File not found: " << file_list.Data() << std::endl; return fDataType;
    }

  // Open the file
  ifstream in; in.open ( file_list.Data() );
  Int_t count = 0;
  TString line;

  while ( in.good() ) {
    in >> line;
    if ( line.IsNull() || line.BeginsWith ( "#" ) ) continue;
    if ( count++ == 1 ) break; // look only over first file;

    TString file ( line );

    // STRICT ORDERING!!! If multiple tree types found last valid will be preffered
    if (IsTreeType(file,"TreeK"))      { fDataType = "kine";    }
    if (IsTreeType(file,"TE"))         { fDataType = "galice";  }
    if (IsTreeType(file,"HLTESDTree")) { fDataType = "hltesd";  }
    if (IsTreeType(file,"esdTree"))    { fDataType = "esd";     }
    if (IsTreeType(file,"aodTree"))    { fDataType = "aod";     }

    return fDataType;
    }
  in.close();
  return fDataType ;
}

//______________________________________________________________________________
TString GetInputDataPath ( const TString& file_list) {
  std::string line_str;
  TString line = line_str.c_str();

  if ( gSystem->AccessPathName ( file_list.Data() ) ) {
    std::cout << "File not found: " << file_list.Data() << std::endl; return line; }

  // Open the file
  ifstream in; in.open ( file_list.Data() );
  Int_t count = 0;

  while ( in.good() ) {
    in >> line_str;
    line = line_str.c_str();
    if ( line.IsNull() || line.BeginsWith ( "#" ) ) { continue; }
    if ( count == 1 ) { break; }  // look only over first file;
    count++;
    }
  in.close();
  line = line_str.c_str();
  return line;
}

//______________________________________________________________________________
TString GetPeriod ( const TString& file_path) {
  TString period = "";

  if (!file_path.IsNull()) {
    // split string in tokens (libs)
    TObjArray* tokens_list = file_path.Tokenize("/"); //tokens_list->Compress();
    TIter next_str(tokens_list);
    TObjString* token = NULL;
    while ((token=(TObjString*)next_str())) {
      TString token_str = token->GetString(); token_str.ToLower();
      if ( token_str.Contains("lhc") ) { period = token_str; break; }
      }
    delete tokens_list;
    }
  return period;
}

//______________________________________________________________________________
TString GetPass ( const TString& file_path) {
  TString pass = "";

  if (!file_path.IsNull()) {
    // split string in tokens (libs)
    TObjArray* tokens_list = file_path.Tokenize("/"); //tokens_list->Compress();
    TIter next_str(tokens_list);
    TObjString* token=0;
    while ((token=(TObjString*)next_str())) {
      TString token_str = token->GetString(); token_str.ToLower();
      if ( token_str.Contains("pass") ) { pass = token_str; break; }
      }
    delete tokens_list;
    }

  return pass;
}

//______________________________________________________________________________
Bool_t PeriodIsMC ( const TString& per_str ) {
  TString period = per_str;
  if (!period.IsNull()) {
    period.ToLower();
    if ( period.BeginsWith("lhc") && (period.Length() > 6) ) {return kTRUE;}
  }
  return kFALSE;
}

//______________________________________________________________________________
TVirtualPad* myPadSetUp ( float currentLeft=0.09, float currentTop=0.06, float currentRight=0.06, float currentBottom=0.09) {
  TVirtualPad* currentPad = gPad;
  currentPad->SetLeftMargin(currentLeft);
  currentPad->SetTopMargin(currentTop);
  currentPad->SetRightMargin(currentRight);
  currentPad->SetBottomMargin(currentBottom);
  return currentPad;
  }

TLegend* myLegendSetUp ( TLegend* currentLegend = 0, float currentTextSize=0.07)
  {
  currentLegend->SetTextFont(42);
  currentLegend->SetBorderSize(0);
  currentLegend->SetFillStyle(0);
  currentLegend->SetFillColor(0);
  currentLegend->SetMargin(0.25);
  currentLegend->SetTextSize(currentTextSize);
  currentLegend->SetEntrySeparation(0.5);
  return currentLegend;
  }


//########  ANALYSIS SPECIFIC FUNCTIONS   ####################
//______________________________________________________________
Double_t GetEvNr (THashList* list, Int_t cent = 0) {
  TH1D* h_ev = dynamic_cast<TH1D*>( list->FindObject(Form("histo5_%i",cent)));
  h_ev->SetDirectory(0);
  Double_t nr_events = h_ev->GetEntries();
  delete h_ev;
  return nr_events;
  }

//______________________________________________________________
Double_t GetNrJets (THashList* list, Int_t cent = 0) {
  TH1D* h_nrjets = dynamic_cast<TH1D*>( list->FindObject(Form("histo4c_%i",cent)));
  h_nrjets->SetDirectory(0);
  Double_t nr_jets = h_nrjets->GetEntries();
  delete h_nrjets;
  return nr_jets;
  }

//______________________________________________________________
TH1D* GetH1_normjets (THashList* list, const char* hname, Int_t cent = 0) {
  if (!list) { std::cout << "invalid list" << endl; return NULL;}
  TH1D* h = getH1(list, hname, cent);
  if (!h) {return NULL;}
  NormHn(h, GetNrJets(list,cent));
  return h;
}

//______________________________________________________________
TH1D* GetH1_normev (THashList* list, const char* hname, Int_t cent = 0) {
  if (!list) { std::cout << "invalid list" << endl; return NULL;}
  TH1D* h = getH1(list, hname, cent);
  if (!h) {return NULL;}
  NormHn(h, GetEvNr(list,cent));
  return h;
}

//______________________________________________________________
AliEmcalList* taskDirectory ( TFile* f, const char* tracks, const char* clusters, const char* cells, const char* jettype) {
  TString sTracks (tracks), sClusters (clusters), sCells (cells), sJettype (jettype);

  TString taskDirectory = "JetCDF_" + sTracks;
  if (!sClusters.IsNull()) {taskDirectory += "_" + sClusters;}
  if (!sCells.IsNull()) {taskDirectory += "_" + sCells;}
  taskDirectory += "_CDF" + sJettype;
  if (sTracks.EqualTo("mcparticles")) {taskDirectory += "MC";}
  taskDirectory += "_histos";

  //   std::cout << "taskDirectory = " << taskDirectory.Data() << std::endl;
  AliEmcalList* task_list = dynamic_cast<AliEmcalList*>(f->Get(taskDirectory.Data()));
  if (!task_list) { std::cout << "invalid AliEmcalList" << endl; return NULL;}
  return task_list;
}

//______________________________________________________________
THashList* getCDFlist ( AliEmcalList* task_list, const char* tracks, const char* clusters, const char* jettype, Double_t radius, Int_t ptmin, Int_t ptmax) {
  TString sTracks (tracks), sClusters (clusters), sJettype (jettype);
  if (!task_list) { std::cout << "invalid AliEmcalList" << endl; return NULL;}

  TString jetstrmin = TString::Itoa((Int_t)ptmin,10);
  TString jetstrmax = TString::Itoa((Int_t)ptmax,10);
  TString radiusString = TString::Format("R%03.0f", radius*100.0);

  TString histoDirectory = "Jet_AKT";
  if (sJettype.EqualTo("full"))
    {histoDirectory += "Full";}
  else
    {histoDirectory += "Charged";}

  histoDirectory += radiusString;
  histoDirectory += "_" + sTracks + "_pT0150";
  if (!sClusters.IsNull()) {histoDirectory += "_" + sClusters + "_E0300_pt_scheme";}
  histoDirectory += "_pt_scheme_ptbin" ;
  histoDirectory += "_" + jetstrmin + "_" + jetstrmax;

  //   std::cout << "histoDirectory = " << histoDirectory.Data() << std::endl;
  return dynamic_cast<THashList*>( task_list->FindObject(histoDirectory.Data()) );
  }

//##########################################################################

void myOptions(Int_t lStat=0) {
  // Set gStyle
  int font = 42;

  TStyle* mystyle = new TStyle("Modern","MyStyle");
  mystyle->SetLegendFillColor(0);

  // From plain
  mystyle->SetFrameBorderMode(0);
  mystyle->SetFrameFillColor(0);
  mystyle->SetCanvasBorderMode(0);
  mystyle->SetPadBorderMode(0);
  mystyle->SetPadColor(10);
  mystyle->SetCanvasColor(10);
  mystyle->SetTitleFillColor(10);
  mystyle->SetTitleBorderSize(1);
  mystyle->SetStatColor(10);
  mystyle->SetStatBorderSize(1);
  mystyle->SetLegendBorderSize(1);

  mystyle->SetDrawBorder(0);
  mystyle->SetTextFont(font);
  mystyle->SetStatFont(font);
  mystyle->SetStatFontSize(0.05);
  mystyle->SetStatX(0.97);
  mystyle->SetStatY(0.98);
  mystyle->SetStatH(0.03);
  mystyle->SetStatW(0.3);
  mystyle->SetTickLength(0.02,"y");
  mystyle->SetEndErrorSize(3);
  mystyle->SetLabelSize(0.03,"xyz");
  mystyle->SetLabelFont(font,"xyz");
  mystyle->SetLabelOffset(0.01,"xyz");
  mystyle->SetTitleFont(font,"xyz");
  mystyle->SetTitleOffset(1.0,"xyz");
  mystyle->SetTitleSize(0.04,"xyz");
  mystyle->SetMarkerSize(2.4);
  mystyle->SetPalette(1,0);
  if (lStat) {
    mystyle->SetOptTitle(1);
    mystyle->SetOptStat(11111111);
    mystyle->SetOptFit(1111);
    }
  else {
    mystyle->SetOptTitle(0);
    mystyle->SetOptStat(0);
    mystyle->SetOptFit(0);
    }

  mystyle->cd();
  }

//##########################################################################
void SetAliceStyle(Bool_t graypalette = kFALSE, Bool_t stat = kFALSE) {
  cout << "Setting style!" << endl;

  gStyle->Reset("Plain");
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  if (stat) {
    gStyle->SetOptTitle(1);
    gStyle->SetOptStat(11111111);
    gStyle->SetOptFit(1111);
    }

  if(graypalette)
    { gStyle->SetPalette(8,0); }
  else
    { gStyle->SetPalette(1); }

  gStyle->SetCanvasColor(10);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetFrameLineWidth(1);
  gStyle->SetFrameFillColor(kWhite);
  gStyle->SetPadColor(10);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadLeftMargin(0.1);
  gStyle->SetPadRightMargin(0.02);
  gStyle->SetHistLineWidth(1);
  gStyle->SetHistLineColor(kRed);
  gStyle->SetFuncWidth(2);
  gStyle->SetFuncColor(kGreen);
  gStyle->SetLineWidth(2);
  gStyle->SetLabelSize(0.045,"xyz");
  gStyle->SetLabelOffset(0.01,"y");
  gStyle->SetLabelOffset(0.01,"x");
  gStyle->SetLabelColor(kBlack,"xyz");
  gStyle->SetTitleSize(0.04,"xyz");
  gStyle->SetTitleOffset(1.25,"y");
  gStyle->SetTitleOffset(1.2,"x");
  gStyle->SetTitleFillColor(kWhite);
  gStyle->SetTextSizePixels(26);
  gStyle->SetTextFont(42);
  gStyle->SetMarkerSize(8);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetTickLength(0.02,"X");
  gStyle->SetTickLength(0.02,"Y");

  gStyle->SetLegendBorderSize(0);
  gStyle->SetLegendFillColor(kWhite);
  //  gStyle->SetFillColor(kWhite);
  gStyle->SetLegendFont(42);

}

//######################################################################################################################################
  void CanvasPartition(TCanvas *C,const Int_t Nx,const Int_t Ny, Float_t lMargin, Float_t rMargin, Float_t bMargin, Float_t tMargin) {
  if (!C) return;
  // Setup Pad layout:
  // Setup Pad layout:
  Float_t vSpacing = 0.0;
  Float_t vStep  = (1.- bMargin - tMargin - (Ny-1) * vSpacing) / Ny;
  
  Float_t hSpacing = 0.0;
  Float_t hStep  = (1.- lMargin - rMargin - (Nx-1) * hSpacing) / Nx;
  
  Float_t vposd,vposu,vmard,vmaru,vfactor;
  Float_t hposl,hposr,hmarl,hmarr,hfactor;
  
  for (Int_t i=0;i<Nx;i++) {
    if (i==0) {
      hposl = 0.0;
      hposr = lMargin + hStep;
      hfactor = hposr-hposl;
      hmarl = lMargin / hfactor;
      hmarr = 0.0;
      }
    else if (i == Nx-1) {
      hposl = hposr + hSpacing;
      hposr = hposl + hStep + rMargin;
      hfactor = hposr-hposl;
      hmarl = 0.0;
      hmarr = rMargin / (hposr-hposl);
      }
    else {
      hposl = hposr + hSpacing;
      hposr = hposl + hStep;
      hfactor = hposr-hposl;
      hmarl = 0.0;
      hmarr = 0.0;
      }
    
    for (Int_t j=0;j<Ny;j++) {
      if (j==0) {
        vposd = 0.0;
        vposu = bMargin + vStep;
        vfactor = vposu-vposd;
        vmard = bMargin / vfactor;
        vmaru = 0.0;
        }
      else if (j == Ny-1) {
        vposd = vposu + vSpacing;
        vposu = vposd + vStep + tMargin;
        vfactor = vposu-vposd;
        vmard = 0.0;
        vmaru = tMargin / (vposu-vposd);
        }
      else {
        vposd = vposu + vSpacing;
        vposu = vposd + vStep;
        vfactor = vposu-vposd;
        vmard = 0.0;
        vmaru = 0.0;
        }
      C->cd(0);
      
      TString name = "pad_" + TString::Itoa(i,10) + "_" + TString::Itoa(j,10);
      TPad *pad = (TPad*) gROOT->FindObject(name.Data());
      if (pad) delete pad;
      pad = new TPad(name,"",hposl,vposd,hposr,vposu);
      pad->SetLeftMargin(hmarl);
      pad->SetRightMargin(hmarr);
      pad->SetBottomMargin(vmard);
      pad->SetTopMargin(vmaru);
      
      pad->SetFrameBorderMode(0);
      pad->SetBorderMode(0);
      pad->SetBorderSize(0);
      
      pad->Draw();
    } // parse Ny
  } // parse Nx
}


