# MuonShowers

This package adds a DataFormat and a EDProducer to fill muon shower related information.
It also includes a test EDAnalyzer with an example of how to access the produced data.

## Installation instructions

```bash
cmsrel CMSSW_9_4_7
cd CMSSW_9_4_7/src/
cmsenv

git clone git@github.com:battibass/MuonShowers.git
scramv1 b -j 5

cd MuonShowers/Tools/test
cmsRun muonShowers_cfg.py
```