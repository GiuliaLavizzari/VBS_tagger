# VBS-like event tagging studies

## useful links

## bibliography

* colour flow and hadronic decay tagging: [2112.09650](https://arxiv.org/abs/2112.09650)
* Lund jet plane [1001.50270](https://arxiv.org/abs/1001.50270), [1903.02275](https://arxiv.org/abs/1903.02275), [1807.04758](https://arxiv.org/abs/1807.04758), [LU-TP-88-14](https://lib-extopc.kek.jp/preprints/PDF/1988/8811/8811323.pdf)  
* Jet Dipolarity [1102.1012](https://arxiv.org/abs/1102.1012)
* Jet Colour Ring [2006.10480](https://arxiv.org/abs/2006.10480)
* The D2 variable [1409.6298](https://arxiv.org/abs/1409.6298)
  
## Madgraph environment setup on lxplus at cern

```
source /cvmfs/sft.cern.ch/lcg/views/LCG_99/x86_64-centos8-gcc10-opt/setup.sh
wget https://launchpad.net/mg5amcnlo/3.0/3.5.x/+download/MG5_aMC_v3.5.4.tar.gz
tar xzf MG5_aMC_v3.5.4.tar.gz
rm MG5_aMC_v3.5.4.tar.gz
cp ~govoni/public/mg5_configuration_LCG102_x86_64-centos9-gcc11-opt.txt .
cd MG5_aMC_v3_5_4/input
mv mg5_configuration.txt mg5_configuration_default.txt
ln -s ../../mg5_configuration_LCG102_x86_64-centos9-gcc11-opt.txt ./mg5_configuration.txt
cd ../
./bin/mg5_aMC
install mg5amc_py8_interface
```


