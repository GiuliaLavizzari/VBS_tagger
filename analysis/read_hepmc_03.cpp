/*
c++ -o read_hepmc_03 read_hepmc_03.cpp `root-config --glibs --cflags` `fastjet-config --cxxflags --libs --plugins` -L/cvmfs/sft.cern.ch/lcg/views/LCG_102/x86_64-centos9-gcc11-opt/lib -lDDG4HepMC3 -lHepMC3Event -lHepMCAnalysis -lHepMCEvent -lHepMCfio -lHepMCRflx -lHepMC -lPhotosppHepMC3 -lPhotosppHepMC
*/

#include "HepMC/IO_GenEvent.h"
#include "HepMC/GenEvent.h"

#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
#include <fastjet/Selector.hh>

#include "TH2F.h"
#include "TCanvas.h"

#include "utils.h"

#include <iostream>
#include <fstream>

using namespace std ;
using namespace fastjet ;
//using namespace FastPartons ;

int main(int argc, const char* argv[]) 
{

  if(argc <= 1 ) {
    cout << "Usage: " << argv[0] << " events.hepmc" << endl;
    return 0;
  }
  
  // Loop over events from files and fill histograms
  cout << "Reading file: " << argv[1] << endl ;

  TH2F h_particleMap ("h_particleMap", "", 100, -5., 5., 100, -3.14, 3.14) ; // eta, phy
  TH2F h_energyFlowMap ("h_energyFlowMap", "", 100, -5., 5., 100, -3.14, 3.14) ; // eta, phy
  TH2F h_energyFlowMap_evt ("h_energyFlowMap_evt", "", 100, -5., 5., 100, -3.14, 3.14) ;

  HepMC::IO_GenEvent ascii_in (argv[1], std::ios::in) ;
  HepMC::GenEvent* evt = ascii_in.read_next_event () ;
  int nEvents = 0 ;
  while (evt)
    {
      cout << "Processing event number " << nEvents << endl ;

      //begin particle loop
      for (HepMC::GenEvent::particle_iterator p = evt->particles_begin () ;
	   p != evt->particles_end () ; ++p) 
        {
          if (!isFinal(*p)) continue ;		
          h_particleMap.Fill ((*p)->momentum ().eta (), (*p)->momentum ().phi ()) ;
          h_energyFlowMap.Fill ((*p)->momentum ().eta (), (*p)->momentum ().phi (), (*p)->momentum ().e ()) ;
          if (nEvents != 0) continue ;
	  h_energyFlowMap_evt.Fill ((*p)->momentum ().eta (), (*p)->momentum ().phi (), (*p)->momentum ().e ()) ;
	}      

      delete evt ;
      ascii_in >> evt ;
      ++nEvents ;
    }

  cout << "Read " << nEvents << " events" << endl ;
  TCanvas c1 ;
  h_particleMap.Draw ("colz") ;
  c1.Print ("particleMap.png") ;
  h_energyFlowMap.Draw ("colz") ;
  c1.Print ("energyMap.png") ;
  h_energyFlowMap_evt.Draw ("colz") ;
  c1.Print ("energyMap_evt.png") ;

  return 0 ;
}

