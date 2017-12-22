#if !( defined  (__CINT__) || defined (__CLING__) )

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
#include <TStyle.h>
#include <TPad.h>
#include <TVirtualPad.h>
#include <TCanvas.h>
#include <TSystemDirectory.h>
#include <TInterpreter.h>

#include <TString.h>
#include <TObjString.h>

//#include <AliEmcalList.h>

#endif

using namespace std;

//void ROOT_FUNCTIONS(){ }

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

//______________________________________________________________
char* ObjGetPtr ( void* ptr /*or if on stack use &obj */) {
  if (!ptr) {return NULL;}
  std::stringstream obj_str;
  obj_str << ptr; // record the pointer
  return Form( "%s", obj_str.str().c_str() );
}

//______________________________________________________________
TString ObjGetName (TObject* obj) {
  TString class_name ("");
  TString* jagmrhmmpdjwikz_str_ptr = NULL;
  TInterpreter* my_int = TInterpreter::Instance();

  // get the class name
  my_int->ProcessLine(Form( "jagmrhmmpdjwikz_str_ptr = reinterpret_cast<TString*>(%s) ; *jagmrhmmpdjwikz_str_ptr = ( (reinterpret_cast<TObject*>(%s))->ClassName() );" , ObjGetPtr(&class_name), ObjGetPtr(obj) )) ;
  class_name.ReplaceAll("\"","");

  jagmrhmmpdjwikz_str_ptr = NULL;
  return class_name;
}

//______________________________________________________________
void ObjGetPtr_howto () {
  cout << "my_int->ProcessLine(Form( \"(( reinterpret_cast<%s*>(%s) )->%s);\" , ObjGetName(obj).Data() , ObjGetPtr(obj), arg )); " << endl;
}

//______________________________________________________________
void ObjGetName_howto () {
  cout << "my_int->ProcessLine(Form( \"(( reinterpret_cast<%s*>(%s) )->%s);\" , ObjGetName(obj).Data() , ObjGetPtr(obj), arg )); " << endl;
}

//______________________________________________________________
static TObject* Apply2Ptr (TObject* obj, const char* arg) {
  // apply the method (arg) to the object
  TInterpreter* my_int = TInterpreter::Instance();
  my_int->ProcessLine(Form( "(( reinterpret_cast<%s*>(%s) )->%s);" , ObjGetName(obj).Data() , ObjGetPtr(obj), arg ));
  return obj;
}

//______________________________________________________________
static void ForEachH (const char* arg = "") {
  if ( std::strlen(arg) == 0 ) {return;}
  if ( gROOT->GetListOfCanvases()->GetEntries() == 0 ) {return;}

  TSeqCollection* list_canvases = gROOT->GetListOfCanvases();
  for (int i=0; i<list_canvases->GetEntries(); ++i) {
    TPad* canvas = dynamic_cast<TPad*> (list_canvases->At(i));
    TList* glist = canvas->GetListOfPrimitives();

    for (int i=0; i<glist->GetEntries(); ++i) {
      TObject* obj = glist->At(i);

      if ( obj->InheritsFrom("TH1") ) { Apply2Ptr (obj, arg ); canvas->Modified(); canvas->Update(); }

      if ( obj->InheritsFrom("TPad") ) {
        TPad* pad = dynamic_cast<TPad*>(obj);
        TList* padlist = pad->GetListOfPrimitives();
        for (int i=0; i<padlist->GetEntries(); ++i) {
          TObject* h_in_padlist = padlist->At(i);
          if ( h_in_padlist->InheritsFrom("TH1") ) { Apply2Ptr (h_in_padlist, arg ); pad->Modified(); pad->Update(); }
          }
        }
    }
  }
}

//______________________________________________________________
static void PadToLogScale() {
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
      }
    }
  gPad->SetLogy();
  gPad->Modified();
}

//______________________________________________________________
static void PadToLogScale_AutoScale() {
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
      (dynamic_cast<TH1*>(obj))->GetMinimumAndMaximum(min,max);
      (dynamic_cast<TH1*>(obj))->GetYaxis()->SetRangeUser(min, max);
      }
    }
  gPad->SetLogy();
  gPad->Modified();
}

//______________________________________________________________
static void PadRebin(Int_t binsgroup) {
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
static void PadSetYaxis(Double_t r1, Double_t r2) {
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
static void PadSetXaxis(Double_t r1, Double_t r2) {
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
TH1D* Rebin (TH1D* h, Int_t nbins, Double_t* xbins) {
return dynamic_cast<TH1D*>( h->Rebin(nbins, Form("%s_rebin",h->GetName()), xbins) );
}

//______________________________________________________________
TH1D* Clone (TH1D* h) {
return dynamic_cast<TH1D*>(h->Clone( Form("%s_cl",h->GetName()) ));
}

//______________________________________________________________
static TH1* NormH ( TH1* h, Option_t* option = "") {
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
static TH1* NormHn ( TH1* h, Double_t norm = 1., Option_t* option = "") {
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
static TH1* ScaleH ( TH1* h, Double_t factor = 1., Option_t* option = "") {
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
static TH1* SetAxis ( TH1* h, const char* axis_name, Float_t l_offset /*Label offset*/, Float_t l_size /*Label size*/, Float_t t_offset /*Title offset*/, Float_t t_size /*Title size*/) {
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
static TH1* SetXProperties ( TH1* h, Float_t l_offset /*Label offset*/, Float_t l_size /*Label size*/, Float_t t_offset /*Title offset*/, Float_t t_size /*Title size*/) {
  return SetAxis (h, "x", l_offset, l_size, t_offset, t_size);
}

//______________________________________________________________
static TH1* SetYProperties ( TH1* h, Float_t l_offset /*Label offset*/, Float_t l_size /*Label size*/, Float_t t_offset /*Title offset*/, Float_t t_size /*Title size*/) {
  return SetAxis (h, "y", l_offset, l_size, t_offset, t_size);
}

//______________________________________________________________
static TH1* SetXRange ( TH1* h, Double_t r1 , Double_t r2) {
  if (!h) {cout << "invalid pointer" << endl; return NULL;}
  h->GetXaxis()->SetRangeUser(r1, r2);
  return h;
}

//______________________________________________________________
static TH1* SetYRange ( TH1* h, Double_t r1 , Double_t r2) {
  if (!h) {cout << "invalid pointer" << endl; return NULL;}
  h->GetYaxis()->SetRangeUser(r1, r2);
  return h;
}

//______________________________________________________________
static TH1* SetXYRange ( TH1* h, Double_t x_r1 , Double_t x_r2, Double_t y_r1 , Double_t y_r2) {
  if (!h) {cout << "invalid pointer" << endl; return NULL;}
  h->GetXaxis()->SetRangeUser(x_r1, x_r2);
  h->GetYaxis()->SetRangeUser(y_r1, y_r2);
  return h;
}

//______________________________________________________________
static TH1* StyleColor ( TH1* h, Style_t mystyle = 1, Color_t mycolor = 1, Size_t mysize = 1.3) {
  if (!h) {cout << "invalid pointer" << endl; return NULL;}
  h->SetMarkerStyle(mystyle);
  h->SetMarkerColor(mycolor);
  h->SetMarkerSize(mysize);
  return h;
}

//______________________________________________________________
static TH1* hDraw ( TH1* h, TString arg = "PE SAME" ) {
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

