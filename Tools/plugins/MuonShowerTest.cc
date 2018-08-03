//////////////////////////////////////
// Ntuplizer that fills muon_pog trees
//////////////////////////////////////

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Event.h" 
#include "FWCore/Framework/interface/EventSetup.h"

#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Common/interface/PtrVector.h"

#include "DataFormats/L1Trigger/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"
#include "MuonShowers/DataFormats/interface/MuonShowerInfo.h"

#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"

#include "TH1F.h"

#include <algorithm>
#include <iostream>

class MuonShowerTest : public edm::EDAnalyzer 
{
public:

  MuonShowerTest(const edm::ParameterSet &);
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void beginRun(const edm::Run&, const edm::EventSetup&);
  virtual void beginJob();
  virtual void endJob();
  
private:
  
  edm::EDGetTokenT<edm::View<reco::Muon> > muonToken_;
  edm::EDGetTokenT<MuonShowerInfo> showerToken_;

  std::map<std::string,TH1F*> plots_;
  
};


MuonShowerTest::MuonShowerTest( const edm::ParameterSet & cfg )
{

  // Input collections
  edm::InputTag tag = cfg.getUntrackedParameter<edm::InputTag>("muonTag", edm::InputTag("muons"));
  if (tag.label() != "none") muonToken_ = consumes<edm::View<reco::Muon> >(tag);

  tag = cfg.getUntrackedParameter<edm::InputTag>("showerTag", edm::InputTag("muonShowerProducer"));
  if (tag.label() != "none") showerToken_ = consumes<MuonShowerInfo>(tag);

}


void MuonShowerTest::beginJob() 
{
  
  edm::Service<TFileService> fs;

  plots_["hNDigisVsPhi_MB1"] = fs->make<TH1F>("hNDigisVsPhi_MB1","hNDigisVsPhi_MB1",48,-TMath::Pi(),TMath::Pi());
  plots_["hNDigisVsPhi_MB2"] = fs->make<TH1F>("hNDigisVsPhi_MB2","hNDigisVsPhi_MB2",48,-TMath::Pi(),TMath::Pi());
  plots_["hNDigisVsPhi_MB3"] = fs->make<TH1F>("hNDigisVsPhi_MB3","hNDigisVsPhi_MB3",48,-TMath::Pi(),TMath::Pi());
  plots_["hNDigisVsPhi_MB4"] = fs->make<TH1F>("hNDigisVsPhi_MB4","hNDigisVsPhi_MB4",48,-TMath::Pi(),TMath::Pi());

}


void MuonShowerTest::beginRun(const edm::Run & run, const edm::EventSetup & config )
{
  
}


void MuonShowerTest::endJob() 
{

}


void MuonShowerTest::analyze(const edm::Event & ev, const edm::EventSetup & config)
{

  // Get muons  
  edm::Handle<edm::View<reco::Muon> > muons;
  if (!muonToken_.isUninitialized() ) 
    { 
      if (!ev.getByToken(muonToken_, muons)) 
	edm::LogError("") << "[MuonShowerTest] Muon collection does not exist !!!";
    }

  // Get shower info  
  edm::Handle<MuonShowerInfo> showerInfo;
  if (!showerToken_.isUninitialized() ) 
    { 
      if (!ev.getByToken(showerToken_, showerInfo)) 
	edm::LogError("") << "[MuonShowerTest] Muon shower info collection does not exist !!!";
    }

  edm::View<reco::Muon>::const_iterator muonIt  = muons->begin();
  edm::View<reco::Muon>::const_iterator muonEnd = muons->end();
 
  for (std::size_t iMu = 0; muonIt != muonEnd; ++muonIt , ++iMu) 
    {
      
      const reco::Muon& mu = (*muonIt);
      edm::Ptr<reco::Muon> muPtr = muons->ptrAt(iMu);

      double muonPhi = mu.phi();

      for (const auto & chamberDigi : (showerInfo->chamberDigis)[muPtr])
	{
	  DetId muonId = chamberDigi.id();

	  if ( !(muonId.det() == DetId::Muon &&
		 muonId.subdetId() == MuonSubdetId::DT) )
	    continue;

	  DTChamberId muonDtId(muonId.rawId());

	  // std::cout << muonDtId << " " 
	  // 	    << chamberDigi.phiViewNDigis(MuonChamberDigi::Range::allCh) << " "
	  // 	    << chamberDigi.phiViewNDigis(MuonChamberDigi::Range::dX50)  << " "
	  // 	    << chamberDigi.phiViewNDigis(MuonChamberDigi::Range::dX25)  << " "
	  // 	    << chamberDigi.phiViewNDigis(MuonChamberDigi::Range::dX15)  << std::endl;
	  
	  if (chamberDigi.phiViewNDigis(MuonChamberDigi::Range::allCh) > 8)
	    {
	      if (muonDtId.station() == 1) plots_["hNDigisVsPhi_MB1"]->Fill(muonPhi);
	      if (muonDtId.station() == 2) plots_["hNDigisVsPhi_MB2"]->Fill(muonPhi);
	      if (muonDtId.station() == 3) plots_["hNDigisVsPhi_MB3"]->Fill(muonPhi);
	      if (muonDtId.station() == 4) plots_["hNDigisVsPhi_MB4"]->Fill(muonPhi);
	    }  
	}
    }

}

#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(MuonShowerTest);
