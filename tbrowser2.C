#include <TString.h>
#include <TFile.h>

void tbrowser2 ( cons char input = "" ) {

TString arg (input);

cout << arg << endl;

gApplication->Terminate();

//TFile* f = new TFile (file);

//TBrowser ( "asd", f);
}

