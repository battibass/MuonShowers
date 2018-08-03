import FWCore.ParameterSet.Config as cms

muonShowerTest = cms.EDAnalyzer("MuonShowerTest",
                                showerTag = cms.untracked.InputTag("muonShowerProducer"),
                                MuonTag   = cms.untracked.InputTag("muons"),
                                )

