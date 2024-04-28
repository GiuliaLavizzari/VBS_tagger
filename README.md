# VBS-like event tagging studies

## useful links


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


