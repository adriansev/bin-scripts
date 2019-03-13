{
gSystem->AddIncludePath("-Wno-deprecated -I. -I$ALICE_PHYSICS/include -I$ALICE_ROOT/include -I$FASTJET/include -I/usr/include");
gSystem->AddDynamicPath("$ALICE_PHYSICS/lib:$ALICE_ROOT/lib:$ROOTSYS/lib:$FASTJET/lib:/usr/lib64");

//gSystem->Load("/usr/lib64/libCGAL");
//gSystem->Load("libfastjet");
//gSystem->Load("libsiscone");
//gSystem->Load("libsiscone_spherical");
//gSystem->Load("libfastjetplugins");
//gSystem->Load("libfastjettools");
//gSystem->Load("libfastjetcontribfragile");
gSystem->Load("libPWGPP");
gSystem->Load("libPWGJEEMCALJetTasks");
//gSystem->ListLibraries();
}



