{
gSystem->AddIncludePath("-I/usr/include");
gSystem->AddDynamicPath("/usr/lib64");
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

