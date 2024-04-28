/*
c++ -o read_hepmc_02 read_hepmc_02.cpp `fastjet-config --cxxflags --libs --plugins` -L/cvmfs/sft.cern.ch/lcg/views/LCG_102/x86_64-centos9-gcc11-opt/lib -lDDG4HepMC3 -lHepMC3Event -lHepMCAnalysis -lHepMCEvent -lHepMCfio -lHepMCRflx -lHepMC -lPhotosppHepMC3 -lPhotosppHepMC
*/

#include "HepMC/IO_GenEvent.h"
#include "HepMC/GenEvent.h"

#include "utils.h"

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
      cout << "Processing event number " << nEvents << endl ;
      //begin particle loop
      int nFinalParticles = 0 ;
      int nZ = 0 ;
      int nZFinal = 0 ;
      int nBeam = 0 ;
      for (HepMC::GenEvent::particle_iterator p = evt->particles_begin () ;
	   p != evt->particles_end () ; ++p) 
        {
	  if (isFinal(*p)) ++nFinalParticles ;	
	  if (isFinal(*p) && isZ (*p)) ++nZFinal ;	
	  //if ((*p)->status() == 1 && isZ (*p)) ++nZFinal ;	
	  if (isZ (*p)) 
	    {
              cout << "        Z particle status: " << (*p)->status() << "\n" ; 
              if ((*p)->status() == 22) cout << "        `-> pz = " << (*p)->momentum().pz() << "\n" ;
	      ++nZ ;
	    }
          if ((*p)->status() == 4)
	    {	 
              // questi sono i protoni, non i partoni effettivamente in collisione, 
	      // perche' hanno momento uguale al fascio impostato in madgraph    
	      // cout << "        beam pz = " << (*p)->momentum().pz() << "\n" ;   
	      ++nBeam ;
            }	      
	}      

      cout << " `+-> found " << nFinalParticles << " particles\n" ;
      cout << "  +-> found " << nZ << " Z's\n" ;
      cout << "  +-> found " << nZFinal << " final state Z's\n" ;
      cout << "  +-> found " << nBeam << " beam particles\n" ;

      delete evt ;
      ascii_in >> evt ;
      ++nEvents ;
    }

  cout << "Read " << nEvents << " events" << endl ;
    return 0;
}

