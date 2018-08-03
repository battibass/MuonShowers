import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

import subprocess
import sys

options = VarParsing.VarParsing()

options.register('globalTag',
                 '94X_dataRun2_ReReco_EOY17_v2', #default value
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "Global Tag")

options.register('nEvents',
                 50000, #default value
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "Maximum number of processed events")

options.register('eosInputFolder',
                 '/store/data/Run2017F/SingleMuon/RAW-RECO/ZMu-17Nov2017-v1/00000/', #default value
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "EOS folder with input files")

options.register('outputFileName',
                 './muonPOGShowers.root', #default value
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "Folder and name ame for output ntuple")

options.parseArguments()

process = cms.Process("MUONSHOWERS")

process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.MessageLogger.cerr.FwkReport.reportEvery = 1
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(options.nEvents))

process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")
process.load("Configuration.StandardSequences.MagneticField_38T_cff")
process.load('Configuration.StandardSequences.MagneticField_cff')

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')

process.GlobalTag.globaltag = cms.string(options.globalTag)

process.source = cms.Source("PoolSource",
                            
        fileNames = cms.untracked.vstring(),
        secondaryFileNames = cms.untracked.vstring()

)

process.TFileService = cms.Service("TFileService", 
                                   fileName = cms.string("muonShower.root")
                                   )

files = subprocess.check_output([ "/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select", "ls", options.eosInputFolder ])
process.source.fileNames = [options.eosInputFolder+"/"+f for f in files.split() ]  

process.load('Configuration.StandardSequences.RawToDigi_cff')

process.load("MuonShowers.Tools.muonShowerProducer_cfi")
process.load("MuonShowers.Tools.muonShowerTest_cfi")

process.out = cms.OutputModule("PoolOutputModule",
                               outputCommands = cms.untracked.vstring('keep *'),
                               fileName = cms.untracked.string(options.outputFileName)
                               )

process.muonShowersPath = cms.Path(process.muonDTDigis + process.muonShowerProducer + process.muonShowerTest)
#process.outPath = cms.EndPath(process.out)
