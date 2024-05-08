// source /cvmfs/sft.cern.ch/lcg/views/LCG_105/x86_64-centos7-gcc12-opt/setup.sh
// g++ shower_ntuplizer.cc -o shower_ntuplizer.exe -O2 -std=c++11 -pedantic -W  `pythia8-config --cxxflags --libs` `fastjet-config --cxxflags --libs --plugins`  `HepMC3-config --cxxflags --libs --plugins` `root-config --cflags --glibs`
// ./shower_ntuplizer.exe events.lhe > out.txt

// main42.cc is a part of the PYTHIA event generator.
// Copyright (C) 2023 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL v2 or later, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// Authors: Mikhail Kirsanov <Mikhail.Kirsanov@cern.ch>

#include "HepMC3/LHEF.h"
#include "HepMC3/LHEFAttributes.h"

#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/HepMC3.h"

#include "TFile.h"
#include "TTree.h"

#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
#include <fastjet/Selector.hh>
#include "utils_hepmc3.h"
#include <string>

using namespace Pythia8;
using namespace fastjet;

const Int_t MAX_PARTICLES = 3000;
const Int_t MAX_SLIMMED_PARTICLES = 30;
const Int_t MAX_LHE = 20;

void setup_pythia(Pythia &pythia, char *filename)
{
  pythia.readString("Main:numberOfEvents = 10");
  pythia.readString("Main:timesAllowErrors = 3");
  pythia.readString("Beams:frameType = 4");
  pythia.readString("Beams:LHEF = " + std::string(filename));
  pythia.readString("Init:showChangedSettings = off");
  pythia.readString("Init:showAllSettings = off");
  pythia.readString("Init:showChangedParticleData = off");
  pythia.readString("Init:showAllParticleData = off");
  pythia.readString("Next:numberCount = 1000");
  pythia.readString("Next:numberShowLHA = 0");
  pythia.readString("Next:numberShowInfo = 0");
  pythia.readString("Next:numberShowProcess = 0");
  pythia.readString("Next:numberShowEvent = 0");
  pythia.readString("Stat:showPartonLevel = off");
}

int main(int argc, char *argv[])
{
  // Check that correct number of command-line arguments
  if (argc != 2)
  {
    cerr << " Unexpected number of command-line arguments. \n You are"
         << " expected to provide one input file name. \n"
         << " Program stopped! " << endl;
    return 1;
  }

  double R_jet = 0.4;
  JetDefinition jet_def(antikt_algorithm, R_jet);
  Selector select_akt = SelectorAbsEtaMax(5.0) && SelectorPtMin(5.);

  vector<PseudoJet> hadrons;

  TFile f("tree.root", "recreate");
  TTree Events("Events", "Events");
  Float_t LHEPart_pt[MAX_LHE], LHEPart_eta[MAX_LHE], LHEPart_phi[MAX_LHE], LHEPart_mass[MAX_LHE];
  Int_t LHEPart_pdgId[MAX_LHE], LHEPart_status[MAX_LHE];
  Int_t nLHEPart;

  Events.Branch("nLHEPart", &nLHEPart, "nLHEPart/I");
  Events.Branch("LHEPart_pt", LHEPart_pt, "[nLHEPart]/F");
  Events.Branch("LHEPart_eta", LHEPart_eta, "[nLHEPart]/F");
  Events.Branch("LHEPart_phi", LHEPart_phi, "[nLHEPart]/F");
  Events.Branch("LHEPart_mass", LHEPart_mass, "[nLHEPart]/F");
  Events.Branch("LHEPart_pdgId", LHEPart_pdgId, "[nLHEPart]/I");
  Events.Branch("LHEPart_status", LHEPart_status, "[nLHEPart]/I");

  Float_t GenJet_pt[MAX_SLIMMED_PARTICLES], GenJet_eta[MAX_SLIMMED_PARTICLES], GenJet_phi[MAX_SLIMMED_PARTICLES], GenJet_mass[MAX_SLIMMED_PARTICLES];
  Int_t nGenJet;

  Events.Branch("nGenJet", &nGenJet, "nGenJet/I");
  Events.Branch("GenJet_pt", GenJet_pt, "[nGenJet]/F");
  Events.Branch("GenJet_eta", GenJet_eta, "[nGenJet]/F");
  Events.Branch("GenJet_phi", GenJet_phi, "[nGenJet]/F");
  Events.Branch("GenJet_mass", GenJet_mass, "[nGenJet]/F");

  Float_t GenPart_pt[MAX_PARTICLES], GenPart_eta[MAX_PARTICLES], GenPart_phi[MAX_PARTICLES], GenPart_mass[MAX_PARTICLES];
  Int_t GenPart_pdgId[MAX_PARTICLES];
  Int_t nGenPart;

  Events.Branch("nGenPart", &nGenPart, "nGenPart/I");
  Events.Branch("GenPart_pt", GenPart_pt, "[nGenPart]/F");
  Events.Branch("GenPart_eta", GenPart_eta, "[nGenPart]/F");
  Events.Branch("GenPart_phi", GenPart_phi, "[nGenPart]/F");
  Events.Branch("GenPart_mass", GenPart_mass, "[nGenPart]/F");
  Events.Branch("GenPart_pdgId", GenPart_pdgId, "[nGenPart]/F");

  Int_t Map_genJetIdx[MAX_PARTICLES], Map_genPartIdx[MAX_PARTICLES];
  Int_t nMap;

  Events.Branch("nMap", &nMap, "nMap/I");
  Events.Branch("Map_genJetIdx", Map_genJetIdx, "[nMap]/I");
  Events.Branch("Map_genPartIdx", Map_genPartIdx, "[nMap]/I");

  // Check that the provided input name corresponds to an existing file.
  ifstream is(argv[1]);
  if (!is)
  {
    cerr << " Command-line file " << argv[1] << " was not found. \n"
         << " Program stopped! " << endl;
    return 1;
  }

  // Confirm that external files will be used for input and output.
  cout << "\n >>> PYTHIA settings will be read from file " << argv[1]
       << endl;

  // Interface for conversion from Pythia8::Event to HepMC event.
  // Specify file where HepMC events will be stored.
  // Pythia8ToHepMC toHepMC(argv[2]);
  HepMC3::Pythia8ToHepMC3 toHepMC;

  // Generator.
  Pythia pythia;

  // Read in commands from external file.
  // pythia.readFile(argv[1]);
  setup_pythia(pythia, argv[1]);
  LHEF::Reader reader(argv[1]);

  // Extract settings to be used in the main program.
  int nEvent = pythia.mode("Main:numberOfEvents");
  int nAbort = pythia.mode("Main:timesAllowErrors");

  // Initialization.
  pythia.init();

  HepMC3::GenEvent *geneve;

  // Begin event loop.
  int iAbort = 0;
  for (int iEvent = 0; iEvent < nEvent; ++iEvent)
  {

    // Generate event.
    if (!(pythia.next() && reader.readEvent()))
    {

      // If failure because reached end of file then exit event loop.
      if (pythia.info.atEndOfFile())
      {
        cout << " Aborted since reached end of Les Houches Event File\n";
        break;
      }

      // First few failures write off as "acceptable" errors, then quit.
      if (++iAbort < nAbort)
        continue;
      cout << " Event generation aborted prematurely, owing to error!\n";
      break;
    }

    // reader.readEvent();
    nLHEPart = 0;
    std::shared_ptr<HepMC3::HEPEUPAttribute> hepe = std::make_shared<HepMC3::HEPEUPAttribute>();
    if (reader.outsideBlock.length())
    {
      hepe->tags = LHEF::XMLTag::findXMLTags(reader.outsideBlock);
    }
    hepe->hepeup = reader.hepeup;
    for (int i = 0; i < hepe->hepeup.NUP; ++i)
    {
      auto m = hepe->momentum(i);
      LHEPart_pt[nLHEPart] = m.pt();
      LHEPart_eta[nLHEPart] = m.eta();
      LHEPart_phi[nLHEPart] = m.phi();
      LHEPart_mass[nLHEPart] = m.m();
      LHEPart_pdgId[nLHEPart] = hepe->hepeup.IDUP[i];
      LHEPart_status[nLHEPart] = hepe->hepeup.ISTUP[i];
      nLHEPart++;
    }

    geneve = new HepMC3::GenEvent;
    toHepMC.fill_next_event(pythia, geneve);

    int status;
    double px, py, pz, e, mass;

    hadrons.clear();
    nGenPart = 0;
    nGenJet = 0;
    nMap = 0;

    for (HepMC3::ConstGenParticlePtr p : geneve->particles())
    {
      if (!isFinal(p))
      {
        continue;
      }
      if (isVisible(p))
      {
        GenPart_pt[nGenPart] = p->momentum().pt();
        GenPart_eta[nGenPart] = p->momentum().eta();
        GenPart_phi[nGenPart] = p->momentum().phi();
        GenPart_mass[nGenPart] = p->momentum().m();
        GenPart_pdgId[nGenPart] = p->pid();
        px = p->momentum().px();
        py = p->momentum().py();
        pz = p->momentum().pz();
        e = p->momentum().e();
        PseudoJet tmp(px, py, pz, e);
        tmp.set_user_index(nGenPart);
        hadrons.push_back(tmp);
        nGenPart++;
      }
    }

    ClusterSequence cluster(hadrons, jet_def);
    vector<PseudoJet> antikT_jets = sorted_by_pt(select_akt(cluster.inclusive_jets()));
    std::cout << "    +-> clustered jet number: " << antikT_jets.size() << "\n";
    for (unsigned i = 0; i < antikT_jets.size(); i++)
    {
      GenJet_pt[nGenJet] = antikT_jets[i].pt();
      GenJet_eta[nGenJet] = antikT_jets[i].eta();
      GenJet_phi[nGenJet] = antikT_jets[i].phi();
      GenJet_mass[nGenJet] = antikT_jets[i].m();
      vector<PseudoJet> constituents = antikT_jets[i].constituents();
      for (unsigned j = 0; j < constituents.size(); j++)
      {
        Map_genJetIdx[nMap] = nGenJet;
        Map_genPartIdx[nMap] = constituents[j].user_index();
        nMap++;
      }
      nGenJet++;
    }

    Events.Fill();
    delete geneve;
    // End of event loop. Statistics.
  }
  Events.Write();
  pythia.stat();

  // Done.
  return 0;
}
