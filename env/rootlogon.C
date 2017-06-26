{
gSystem->AddIncludePath("-Wno-deprecated -I$. -I/usr/include -I$FASTJET/include -I$ROOTSYS/include -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include");
gSystem->AddDynamicPath("/usr/lib64 $FASTJET/lib $ROOTSYS/lib $ALICE_ROOT/lib $ALICE_PHYSICS/lib");

gSystem->Load("libCGAL");
gSystem->Load("libfastjet");
gSystem->Load("libsiscone");
gSystem->Load("libsiscone_spherical");
gSystem->Load("libfastjetplugins");
gSystem->Load("libfastjettools");
gSystem->Load("libfastjetcontribfragile");
gSystem->Load("libPWGJE");
gSystem->Load("libPWGJEEMCALJetTasks");
//gSystem->ListLibraries();
}

