# VBS-like event tagging studies

## useful links

## bibliography

* colour flow and hadronic decay tagging: [2112.09650](https://arxiv.org/abs/2112.09650)
* Lund jet plane [1001.50270](https://arxiv.org/abs/1001.50270), [1903.02275](https://arxiv.org/abs/1903.02275), [1807.04758](https://arxiv.org/abs/1807.04758), [LU-TP-88-14](https://lib-extopc.kek.jp/preprints/PDF/1988/8811/8811323.pdf)
* Jet Dipolarity [1102.1012](https://arxiv.org/abs/1102.1012)
* Jet Colour Ring [2006.10480](https://arxiv.org/abs/2006.10480)
* The D2 variable [1409.6298](https://arxiv.org/abs/1409.6298)

## Madgraph environment setup on lxplus at cern

```shell
git clone git@github.com:{your_github_username}/VBS_tagger.git
cd VBS_tagger/
# on centos7
source /cvmfs/sft.cern.ch/lcg/views/LCG_102/x86_64-centos7-gcc11-opt/setup.sh
wget https://launchpad.net/mg5amcnlo/3.0/3.5.x/+download/MG5_aMC_v2.9.20.tar.gz
tar xzf MG5_aMC_v2.9.20.tar.gz
rm MG5_aMC_v2.9.20.tar.gz
mv MG5_aMC_v2_9_20/input/mg5_configuration.txt MG5_aMC_v2_9_20/input/mg5_configuration_default.txt
cp generation/mg5_configuration_LCG102_x86_64-centos7-gcc11-opt.txt MG5_aMC_v2_9_20/input/mg5_configuration.txt
./MG5_aMC_v2_9_20/bin/mg5_aMC #this enters madgraph and will show a prompt like: MG5_aMC>
```

## Run the shower and the analysis starting from LHE

```bash
source /cvmfs/sft.cern.ch/lcg/views/LCG_102/x86_64-centos7-gcc11-opt/setup.sh
```
Now you can compile `shower_ntuplizer.cc` in the analysis folder:
```bash
cd analysis/

g++ shower_ntuplizer.cc -o shower_ntuplizer -O2 -std=c++11 -pedantic -W  `pythia8-config --cxxflags --libs` `fastjet-config --cxxflags --libs --plugins`  `HepMC3-config --cxxflags --libs --plugins` `root-config --cflags --glibs`
```
In order to execute `shower_ntuplizer` you have to pass it the path to a LHE file

```bash
./shower_ntuplizer.exe events.lhe > out.txt
```

Now `tree.root` will be created containing a TTree Events containing the following collections

* LHEPart:
    * pt
    * eta
    * phi
    * mass
    * pdgId
    * status (LHE status of a particle)

* GenPart:
    * pt
    * eta
    * phi
    * mass
    * pdgId

* GenJet: clustered with anti-kT R=0.4 of all visible GenParticles
    * pt
    * eta
    * phi
    * mass


* Map: mapping of each component of a GenJet to a GenPart
    * genJetIdx
    * genPartIdx
