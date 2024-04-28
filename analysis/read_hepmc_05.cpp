/*
c++ -o read_hepmc_05 read_hepmc_05.cpp `fastjet-config --cxxflags --libs --plugins` -L/cvmfs/sft.cern.ch/lcg/views/LCG_102/x86_64-centos9-gcc11-opt/lib -lHepMCAnalysis -lHepMCEvent -lHepMCfio -lHepMCRflx -lHepMC -lPhotosppHepMC
*/

#include "HepMC/IO_GenEvent.h"
#include "HepMC/GenEvent.h"

#include "utils.h"

#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
#include <fastjet/Selector.hh>

#include <iostream>
#include <fstream>

using namespace fastjet ;
using namespace std ;

int main(int argc, const char* argv[]) 
{

  if(argc <= 1 ) {
    cout << "Usage: " << argv[0] << " events.hepmc" << endl;
    return 0;
  }
  
  // Loop over events from files and fill histograms
  cout << "Reading file: " << argv[1] << endl ;

  double R_jet = 0.4;
  JetDefinition jet_def(antikt_algorithm, R_jet);
  Selector select_akt = SelectorAbsEtaMax(5.0) && SelectorPtMin(20.);

  HepMC::IO_GenEvent ascii_in (argv[1], std::ios::in) ;
  HepMC::GenEvent* evt = ascii_in.read_next_event () ;
  int nEvents = 0 ;
  while (evt)
    {
      cout << "Processing event number " << nEvents << endl ;

      vector<PseudoJet> hadrons, electrons, muons, photons;

      electrons.clear () ;
      muons.clear () ;
      photons.clear () ;
      hadrons.clear () ;
      
      //begin particle loop
      for (HepMC::GenEvent::particle_iterator p = evt->particles_begin () ;
           p != evt->particles_end () ; ++p ) 
        {
        
          //get final state particles:
  
          //hadrons and b-tagging
          if (isHadron (*p) && isFinal (*p))
            {
//              int btag = isBHadron (*p) ? 1 : 0 ;
              PseudoJet tmp ( (*p)->momentum ().px (), (*p)->momentum ().py (), (*p)->momentum ().pz (), (*p)->momentum ().e ()) ;
//              tmp.set_user_index (btag) ;
              hadrons.push_back (tmp) ;
            }
        
          //photons
          if (isPhoton (*p) && isFinal (*p))
            {
              PseudoJet tmp ( (*p)->momentum ().px (), (*p)->momentum ().py (), (*p)->momentum ().pz (), (*p)->momentum ().e ()) ;
              photons.push_back (tmp) ;
            }
          
          //electrons
          if (isElectron (*p) && isFinal (*p))
            {
              PseudoJet tmp ( (*p)->momentum ().px (), (*p)->momentum ().py (), (*p)->momentum ().pz (), (*p)->momentum ().e ()) ;
              electrons.push_back (tmp) ;
            }
          
          //muons
          if (isMuon (*p) && isFinal (*p))
            {
              PseudoJet tmp ( (*p)->momentum ().px (), (*p)->momentum ().py (), (*p)->momentum ().pz (), (*p)->momentum ().e ()) ;
              muons.push_back (tmp) ;
            }
          
        } //end particle loop
           
      //cluster hadrons into jets
      ClusterSequence cluster (hadrons, jet_def) ;
      vector<PseudoJet> antikT_jets = sorted_by_pt (select_akt (cluster.inclusive_jets ())) ;

      cout << "    +-> clustered jet number: " << antikT_jets.size () << "\n" ;

      delete evt ;
      ascii_in >> evt ;
      if (nEvents > 3) break ;
      ++nEvents ;
    }

  cout << "Read " << nEvents << " events" << endl ;
  return 0;
}

