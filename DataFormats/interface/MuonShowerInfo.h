#ifndef MuonShowerInfo_h
#define MuonShowerInfo_h

#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/Common/interface/ValueMap.h"

#include <algorithm>

class MuonChamberDigi
{

 public:
  MuonChamberDigi() { for (int i = 0; i < 4 ; ++i) std::fill_n(m_digi[i], 3, 0); };
  ~MuonChamberDigi() { };

  enum Range { allCh = 0, dX50, dX25, dX15 };

  // get digis in phi view
  // 0 both SL, 1 = SL phi 1, 2 = SL phi 2
  int phiViewNDigis(Range range, int sl = 0) const
    {
      int nPhiDigis = 0;
      if (sl ==0 || sl ==1)
	nPhiDigis += m_digi[range][0];
      if (sl ==0 || sl ==2)
	nPhiDigis += m_digi[range][2];
      return nPhiDigis;
    };

  // get digis in theta view
  int thetaViewNDigis(Range range) const { return m_digi[range][1]; };

  // get digis in both views
  int nDigis(Range range, int sl = 0) const { return phiViewNDigis(range) + thetaViewNDigis(range); }

  // get the DetId
  DetId id() const { return m_id; }
   
  int m_digi[4][3];
  DetId m_id;

};

class MuonShowerInfo
{

 public:
  MuonShowerInfo() { };
  ~MuonShowerInfo() { };
  
  edm::ValueMap<std::vector<MuonChamberDigi> > chamberDigis;

};

#endif
