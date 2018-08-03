//////////////////////////////////////
// Ntuplizer that fills muon_pog trees
//////////////////////////////////////

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Event.h" 
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/EDPutToken.h"

#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Common/interface/PtrVector.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"
#include "DataFormats/MuonDetId/interface/CSCDetId.h"

#include "DataFormats/DTDigi/interface/DTDigi.h"
#include "DataFormats/DTDigi/interface/DTDigiCollection.h"

#include "DataFormats/DTRecHit/interface/DTRecSegment4DCollection.h"

#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"

#include "MuonShowers/DataFormats/interface/MuonShowerInfo.h"

#include <algorithm>
#include <iostream>

class MuonShowerProducer : public edm::EDProducer
{
public:

  MuonShowerProducer(const edm::ParameterSet &);
  
  void produce(edm::Event&, const edm::EventSetup&) override;
  void beginRun(const edm::Run&, const edm::EventSetup&) override;
  
private:  
  
  edm::EDGetTokenT<edm::View<reco::Muon> > muonToken_;

  edm::EDGetTokenT<DTDigiCollection> dtDigiToken_;

  edm::EDPutTokenT<MuonShowerInfo> showerInfoToken_;
  

};


MuonShowerProducer::MuonShowerProducer( const edm::ParameterSet & cfg )
{

  // Input collections
  edm::InputTag tag = cfg.getUntrackedParameter<edm::InputTag>("MuonTag", edm::InputTag("muons"));
  if (tag.label() != "none") muonToken_ = consumes<edm::View<reco::Muon> >(tag);

  tag = cfg.getUntrackedParameter<edm::InputTag>("dtDigiTag", edm::InputTag("muonDTDigis"));
  if (tag.label() != "none") dtDigiToken_ = consumes<DTDigiCollection>(tag);

  produces<MuonShowerInfo>();
    
}


void MuonShowerProducer::beginRun(const edm::Run & run, const edm::EventSetup & config )
{
  
}


void MuonShowerProducer::produce(edm::Event & ev, const edm::EventSetup & config)
{

  std::unique_ptr<MuonShowerInfo> showerInfo(new MuonShowerInfo);

  edm::ESHandle<DTGeometry>  dtGeom;
  config.get<MuonGeometryRecord>().get(dtGeom);

  edm::Handle<edm::View<reco::Muon> > muons;
  if (!muonToken_.isUninitialized() ) 
    { 
      if (!ev.getByToken(muonToken_, muons)) 
	edm::LogError("") << "[MuonShowerProducer] Muon collection does not exist !!!";
    }

  edm::Handle<DTDigiCollection> dtDigis;
  if (!dtDigiToken_.isUninitialized() )
    {
      if (!ev.getByToken(dtDigiToken_,dtDigis))
	edm::LogError("") << "[MuonShowerProducer] DT digis collection does not exist !!!";
    }

  std::vector<std::vector<MuonChamberDigi> > chamberDigis(muons->size());

  if (muons.isValid() && dtDigis.isValid() && dtGeom.isValid() )
    { 

      edm::View<reco::Muon>::const_iterator muonIt  = muons->begin();
      edm::View<reco::Muon>::const_iterator muonEnd = muons->end();

      for (std::size_t iMu = 0; muonIt != muonEnd; ++muonIt , ++iMu) 
	{

	  const reco::Muon& mu = (*muonIt);
	  edm::Ptr<reco::Muon> muPtr = muons->ptrAt(iMu);

	  if ( !mu.isMatchesValid() )
	    continue;

	  for ( const reco::MuonChamberMatch & match : mu.matches() )
	    {

	      DetId muonId = match.id;
	      
	      if ( !(muonId.det() == DetId::Muon && 
		     muonId.subdetId() == MuonSubdetId::DT) )
		continue;

	      DTChamberId muonDtId(muonId.rawId());  

	      MuonChamberDigi chamberDigi;

	      chamberDigi.m_id = muonId;
		  
	      DTDigiCollection::DigiRangeIterator dtLayerIdIt  = dtDigis->begin();
	      DTDigiCollection::DigiRangeIterator dtLayerIdEnd = dtDigis->end();
		  
	      for (; dtLayerIdIt!=dtLayerIdEnd; ++dtLayerIdIt)
		{
		  if ((*dtLayerIdIt).first.chamberId() != muonDtId)
		    continue;

		  DTDigiCollection::const_iterator digiIt  = (*dtLayerIdIt).second.first;
		  DTDigiCollection::const_iterator digiEnd = (*dtLayerIdIt).second.second;
			  
		  for (; digiIt!=digiEnd; ++digiIt)
		    {
		      (*dtLayerIdIt).first.superLayer() == 1 && chamberDigi.m_digi[MuonChamberDigi::Range::allCh][0]++;
		      (*dtLayerIdIt).first.superLayer() == 2 && chamberDigi.m_digi[MuonChamberDigi::Range::allCh][1]++;
		      (*dtLayerIdIt).first.superLayer() == 3 && chamberDigi.m_digi[MuonChamberDigi::Range::allCh][2]++;
		      
		      const auto topo = dtGeom->layer((*dtLayerIdIt).first)->specificTopology();
		      Float_t xWire = topo.wirePosition((*digiIt).wire());
		      
		      Float_t dX = std::abs(match.x - xWire);
		      
		      if (dX < 50.)
			{
			  (*dtLayerIdIt).first.superLayer() == 1 && chamberDigi.m_digi[MuonChamberDigi::Range::dX50][0]++;
			  (*dtLayerIdIt).first.superLayer() == 2 && chamberDigi.m_digi[MuonChamberDigi::Range::dX50][1]++;
			  (*dtLayerIdIt).first.superLayer() == 3 && chamberDigi.m_digi[MuonChamberDigi::Range::dX50][2]++;
			}
		      if (dX < 25.)
			{
			  (*dtLayerIdIt).first.superLayer() == 1 && chamberDigi.m_digi[MuonChamberDigi::Range::dX25][1]++;
			  (*dtLayerIdIt).first.superLayer() == 2 && chamberDigi.m_digi[MuonChamberDigi::Range::dX25][1]++;
			  (*dtLayerIdIt).first.superLayer() == 3 && chamberDigi.m_digi[MuonChamberDigi::Range::dX25][1]++;
			}
		      if (dX < 15.)
			{
			  (*dtLayerIdIt).first.superLayer() == 1 && chamberDigi.m_digi[MuonChamberDigi::Range::dX15][1]++;
			  (*dtLayerIdIt).first.superLayer() == 2 && chamberDigi.m_digi[MuonChamberDigi::Range::dX15][1]++;
			  (*dtLayerIdIt).first.superLayer() == 3 && chamberDigi.m_digi[MuonChamberDigi::Range::dX15][1]++;
			}
		    }
		  
		}
	    
	      chamberDigis.at(iMu).push_back(chamberDigi);

	    }
	}
    }

  typename edm::ValueMap<std::vector<MuonChamberDigi> >::Filler filler(showerInfo->chamberDigis);
  filler.insert(muons, chamberDigis.begin(), chamberDigis.end());
  filler.fill();
    
  ev.put(std::move(showerInfo));

}


#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(MuonShowerProducer);
