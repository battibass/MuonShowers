import FWCore.ParameterSet.Config as cms

muonShowerProducer = cms.EDProducer("MuonShowerProducer",
                                    dtDigiTag = cms.untracked.InputTag("muonDTDigis"),
                                    MuonTag   = cms.untracked.InputTag("muons"),
                                    )

