/*
c++ -o read_hepmc_04 read_hepmc_04.cpp `fastjet-config --cxxflags --libs --plugins` -L/cvmfs/sft.cern.ch/lcg/views/LCG_102/x86_64-centos9-gcc11-opt/lib -lHepMCAnalysis -lHepMCEvent -lHepMCfio -lHepMCRflx -lHepMC -lPhotosppHepMC
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
      int nBeam = 0 ;
      
      //begin particle loop
      for (HepMC::GenEvent::particle_iterator p = evt->particles_begin () ;
	         p != evt->particles_end () ; ++p) 
        {
          if ((*p)->status() == 4)
	          {	 
              // questi sono i protoni, non i partoni effettivamente in collisione, 
	      // perche' hanno momento uguale al fascio impostato in madgraph    
	      cout << "     + beam particle pz = " << (*p)->momentum().pz() << "\n" ;   
              cout << "     + outgoing particles number: " << (*p)->end_vertex ()->particles_out_size () << "\n" ;
              ++nBeam ;
            }	      
	      }      

      cout << "  +-> found " << nBeam << " beam particles\n" ;

      cout << "  +-> signal vtx pointer: " << evt->signal_process_vertex () << "\n" ;
      cout << "  +-> first vtx info in: " << (*evt->vertices_begin ())->particles_in_size () << "\n" ;
      cout << "  +-> first vtx info out: " << (*evt->vertices_begin ())->particles_out_size () << "\n" ;
      
//      cout << "  +-> signal vtx outgoing particles nb: " << evt->signal_process_vertex ()->particles_out_size () << "\n" ;
//      cout << "  +-> signal vtx incoming particles nb: " << evt->signal_process_vertex ()->particles_in_size () << "\n" ;

      cout << "  +-> valid beam particles? " << evt->valid_beam_particles () << "\n" ;
      cout << "  +-> beam particles vector size " << evt->beam_particles ().first->momentum ().z () << "\n" ;

      delete evt ;
      ascii_in >> evt ;
      if (nEvents > 3) break ;
      ++nEvents ;
    }

  cout << "Read " << nEvents << " events" << endl ;
  return 0;
}

