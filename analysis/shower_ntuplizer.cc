// g++ shower_ntuplizer.cc -o shower_ntuplizer -O2 -std=c++11 -pedantic -W `pythia8-config --cxxflags --libs` `fastjet-config --cxxflags --libs --plugins`  `HepMC3-config --cxxflags --libs --plugins` `root-config --cflags --glibs`
// ./shower_ntuplizer events.lhe > out.txt

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
#include <vector>

using namespace Pythia8;
using namespace fastjet;

const long unsigned int MAX_PARTICLES = 3000;
const long unsigned int MAX_SLIMMED_PARTICLES = 3000;
const long unsigned int MAX_LHE = 2000;

void setup_pythia(Pythia &pythia, char *filename) {
  pythia.readString("Main:numberOfEvents = 10000");
  pythia.readString("Main:timesAllowErrors = 10");
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

int main(int argc, char *argv[]) {
  // Check that correct number of command-line arguments
  if (argc != 2) {
    cerr << " Unexpected number of command-line arguments. \n You are"
         << " expected to provide one input file name. \n"
         << " Program stopped! " << endl;
    return 1;
  }

  double R_jet = 0.4;
  JetDefinition jet_def(antikt_algorithm, R_jet);
  Selector select_akt = SelectorAbsEtaMax(5.0) && SelectorPtMin(1.);

  vector<PseudoJet> hadrons;

  TFile f("tree.root", "recreate");
  TTree GenJets("GenJets", "GenJets");
  TTree GenParticles("GenParticles", "GenParticles");
  TTree LHEParticles("LHEParticles", "LHEParticles");

  std::vector<float> LHEPart_pt, LHEPart_eta, LHEPart_phi, LHEPart_mass;
  std::vector<int> LHEPart_pdgId, LHEPart_status;

  LHEParticles.Branch("LHEPart_pt", &LHEPart_pt);
  LHEParticles.Branch("LHEPart_eta", &LHEPart_eta);
  LHEParticles.Branch("LHEPart_phi", &LHEPart_phi);
  LHEParticles.Branch("LHEPart_mass", &LHEPart_mass);
  LHEParticles.Branch("LHEPart_pdgId", &LHEPart_pdgId);
  LHEParticles.Branch("LHEPart_status", &LHEPart_status);

  std::vector<float> GenJet_pt, GenJet_eta, GenJet_phi, GenJet_mass;
  std::vector<int> GenJet_nConstituents, GenJet_constituents;

  GenJets.Branch("GenJet_pt", &GenJet_pt);
  GenJets.Branch("GenJet_eta", &GenJet_eta);
  GenJets.Branch("GenJet_phi", &GenJet_phi);
  GenJets.Branch("GenJet_mass", &GenJet_mass);
  GenJets.Branch("GenJet_nConstituents", &GenJet_nConstituents);
  GenJets.Branch("GenJet_constituents", &GenJet_constituents);

  std::vector<float> GenPart_pt, GenPart_eta, GenPart_phi, GenPart_mass;
  std::vector<int> GenPart_pdgId;
  int nGenPart;

  GenParticles.Branch("nGenPart", &nGenPart);
  GenParticles.Branch("GenPart_pt", &GenPart_pt);
  GenParticles.Branch("GenPart_eta", &GenPart_eta);
  GenParticles.Branch("GenPart_phi", &GenPart_phi);
  GenParticles.Branch("GenPart_mass", &GenPart_mass);
  GenParticles.Branch("GenPart_pdgId", &GenPart_pdgId);

  // Check that the provided input name corresponds to an existing file.
  ifstream is(argv[1]);
  if (!is) {
    cerr << " Command-line file " << argv[1] << " was not found. \n"
         << " Program stopped! " << endl;
    return 1;
  }

  // Confirm that external files will be used for input and output.
  cout << "\n >>> PYTHIA settings will be read from file " << argv[1] << endl;

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
  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {
    std::cout << "iEvent " << iEvent << "\n";

    // Generate event.
    if (!(pythia.next() && reader.readEvent())) {

      // If failure because reached end of file then exit event loop.
      if (pythia.info.atEndOfFile()) {
        cout << " Aborted since reached end of Les Houches Event File\n";
        break;
      }

      // First few failures write off as "acceptable" errors, then quit.
      if (++iAbort < nAbort) continue;
      cout << " Event generation aborted prematurely, owing to error!\n";
      break;
    }

    // reader.readEvent();
    std::shared_ptr<HepMC3::HEPEUPAttribute> hepe =
        std::make_shared<HepMC3::HEPEUPAttribute>();
    if (reader.outsideBlock.length()) {
      hepe->tags = LHEF::XMLTag::findXMLTags(reader.outsideBlock);
    }
    hepe->hepeup = reader.hepeup;
    for (int i = 0; i < hepe->hepeup.NUP; ++i) {
      auto m = hepe->momentum(i);
      LHEPart_pt.push_back(m.pt());
      LHEPart_eta.push_back(m.eta());
      LHEPart_phi.push_back(m.phi());
      LHEPart_mass.push_back(m.m());
      LHEPart_pdgId.push_back(hepe->hepeup.IDUP[i]);
      LHEPart_status.push_back(hepe->hepeup.ISTUP[i]);
    }

    geneve = new HepMC3::GenEvent;
    toHepMC.fill_next_event(pythia, geneve);

    int status;
    double px, py, pz, e, mass;

    hadrons.clear();
    nGenPart = 0;

    for (HepMC3::ConstGenParticlePtr p : geneve->particles()) {
      //std::cout << " #genPart is final " << isFinal(p) << " and visible " << isVisible(p) << "\n";
      if (!isFinal(p)) {
        continue;
      }
      if (isVisible(p)) {
        GenPart_pt.push_back(p->momentum().pt());
        GenPart_eta.push_back(p->momentum().eta());
        GenPart_phi.push_back(p->momentum().phi());
        GenPart_mass.push_back(p->momentum().m());
        GenPart_pdgId.push_back(p->pid());
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
    vector<PseudoJet> antikT_jets =
        sorted_by_pt(select_akt(cluster.inclusive_jets()));
    std::cout << "    +-> clustered jet number: " << antikT_jets.size() << "\n";
    for (unsigned i = 0; i < antikT_jets.size(); i++) {
      GenJet_pt.push_back(antikT_jets[i].pt());
      GenJet_eta.push_back(antikT_jets[i].eta());
      GenJet_phi.push_back(antikT_jets[i].phi());
      GenJet_mass.push_back(antikT_jets[i].m());
      vector<PseudoJet> constituents = antikT_jets[i].constituents();
      GenJet_nConstituents.push_back(constituents.size());
      for (unsigned j = 0; j < constituents.size(); j++) {
        GenJet_constituents.push_back(constituents[j].user_index());
      }
    }

    // Fill trees
    LHEParticles.Fill();
    GenJets.Fill();
    GenParticles.Fill();
    delete geneve;

    LHEPart_pt.clear();
    LHEPart_eta.clear();
    LHEPart_phi.clear();
    LHEPart_mass.clear();
    LHEPart_pdgId.clear();
    LHEPart_status.clear();

    GenJet_pt.clear();
    GenJet_eta.clear();
    GenJet_phi.clear();
    GenJet_mass.clear();

    GenPart_pt.clear();
    GenPart_eta.clear();
    GenPart_phi.clear();
    GenPart_mass.clear();
    GenPart_pdgId.clear();
    GenJet_nConstituents.clear();
    GenJet_constituents.clear();
  }
  LHEParticles.Write();
  GenJets.Write();
  GenParticles.Write();
  pythia.stat();

  // Done.
  return 0;
}
