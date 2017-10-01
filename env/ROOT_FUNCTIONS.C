//#if !( defined  (__CINT__) || defined (__CLING__) )

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

#include "AliEmcalList.h"

//#endif

#undef __ROOT_FUNCTIONS_LOADED__
#define __ROOT_FUNCTIONS_LOADED__ true

using namespace std;

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
void GetObjPtr_howto () {
  cout << "my_int->ProcessLine(Form( \"(( reinterpret_cast<%s*>(%s) )->%s);\" , ObjGetName(obj).Data() , ObjGetPtr(obj), arg )); " << endl;
}

//______________________________________________________________
void GetObjName_howto () {
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
TH1D* Rebin (TH1D* h, Double_t* xbins) {
Int_t nbins = ( sizeof (xbins)/sizeof (xbins[0]) ) - 1;
TString h_name = h->GetName(); h_name += "_rebin";

TH1D* h_rebin = dynamic_cast<TH1D*> (h->Rebin(nbins, h_name.Data(), xbins));
return h_rebin;
}

//______________________________________________________________
TH1F* Rebin (TH1F* h, Double_t* xbins) {
Int_t nbins = ( sizeof (xbins)/sizeof (xbins[0]) ) - 1;
TString h_name = h->GetName(); h_name += "_rebin";

TH1F* h_rebin = dynamic_cast<TH1F*> (h->Rebin(nbins, h_name.Data(), xbins));
return h_rebin;
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
const char* cd ( TString path = ".")
{
 if ( gSystem->ChangeDirectory(path.Data()) )
    { return pwd(); }
 else
    { printf ("Could not change dir"); return pwd(); }
}

