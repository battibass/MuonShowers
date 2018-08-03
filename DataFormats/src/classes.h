
#include "MuonShowers/DataFormats/interface/MuonShowerInfo.h"

namespace DataFormats_MuonShowerInfo 
{
  struct dictionary 
  {
    MuonChamberDigi chDigi_S;
    
    std::vector<MuonChamberDigi> chDigi_V;

    std::vector<std::vector<MuonChamberDigi>> chDigi_VV;

    edm::ValueMap<std::vector<MuonChamberDigi> > chDigi_ValMap;

    MuonShowerInfo showerInfo_S;
    
    edm::Wrapper<MuonShowerInfo> showerInfo_W;
    
  };
}
