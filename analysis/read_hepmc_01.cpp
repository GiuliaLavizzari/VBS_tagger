/*
c++ -o read_hepmc_01 read_hepmc_01.cpp -L/cvmfs/sft.cern.ch/lcg/views/LCG_102/x86_64-centos9-gcc11-opt/lib -lDDG4HepMC3 -lHepMC3Event -lHepMCAnalysis -lHepMCEvent -lHepMCfio -lHepMCRflx -lHepMC -lPhotosppHepMC3 -lPhotosppHepMC
*/

#include "HepMC/IO_GenEvent.h"
#include "HepMC/GenEvent.h"

#include <iostream>
#include <fstream>

using namespace std ;

int main(int argc, const char* argv[]) 
{

  if(argc <= 1 ) {
    cout << "Usage: " << argv[0] << " events.hepmc" << endl;
    return 0;
  }
  
  // Loop over events from files and fill histograms
  cout << "Reading file: " << argv[1] << endl ;


  HepMC::IO_GenEvent ascii_in (argv[1], std::ios::in) ;
  HepMC::GenEvent* evt = ascii_in.read_next_event () ;
  int nEvents = 0 ;
  while (evt)
    {
      cout << "Processing event number " << nEvents << endl;
      delete evt ;
      ascii_in >> evt ;
      ++nEvents ;
    }

  cout << "Read " << nEvents << " events" << endl ;
    return 0;
}

