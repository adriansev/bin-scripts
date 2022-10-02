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

