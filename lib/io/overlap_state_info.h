#ifndef OVERLAP_STATE_INFO_H
#define OVERLAP_STATE_INFO_H

#include "chromabase.h"
#include "io/param_io.h"
#include "io/eigen_io.h"

#include <string>
using namespace QDP;

namespace Chroma
{

  class OverlapStateInfo {
  public:

    virtual const Real& getApproxMin(void) const = 0;
    virtual const Real& getApproxMax(void) const = 0;

    virtual int   getNWilsVec(void) const = 0;
    virtual bool  loadEigVec(void) const = 0;
    virtual bool  computeEigVec(void) const = 0;

    virtual const EigenIO_t& getEigenIO(void) const = 0;
    virtual const RitzParams_t& getRitzParams(void) const = 0;

    // Virtual Destructor
    virtual ~OverlapStateInfo(void) {}
  };

  class ZolotarevStateInfo : public OverlapStateInfo { 
  private:
    bool initedP;
    Real ApproxMin;
    Real ApproxMax;
    int  NWilsVec;
    bool load_eigenP;
    EigenIO_t eigen_io;
    RitzParams_t ritzery;

    void notInited(void) const { 
      QDPIO::cerr << "ZolotarevStateInfo not inited" << endl;
      QDP_abort(1);
    }

    void notLoadEig(void) const { 
      QDPIO::cerr << "No loadable Eigenvalues. No eigen_io member " << endl;
      QDP_abort(1);
    }

    void notComputeEig(void) const {
      QDPIO::cerr << "Eigenvalues should not be computed. No Ritz Member" << endl;
      QDP_abort(1);
    }

  public:


    ZolotarevStateInfo(void);   
  
    void init(const Real& _ApproxMin,
	      const Real& _ApproxMax,
	      const int&  _NWilsVec,
	      const bool& _load_eigenP,
	      const EigenIO_t& _eigen_io,
	      const RitzParams_t& _ritzery) {
      ApproxMin = _ApproxMin;
      ApproxMax =_ApproxMax;
      NWilsVec= _NWilsVec;
      load_eigenP = _load_eigenP;
      eigen_io = _eigen_io;
      ritzery = _ritzery;
      initedP = true; 
    }


    const Real& getApproxMin(void) const { 
      if( ! initedP )
	notInited();
      return ApproxMin;
    } 

    const Real& getApproxMax(void) const { 
      if( ! initedP )
	notInited();
      return ApproxMax; 
    }

    int   getNWilsVec(void) const { 
      if( ! initedP )
	notInited();
      return NWilsVec; 
    }

    bool  loadEigVec(void) const {
      if( ! initedP )
	notInited();
      return ( load_eigenP && (NWilsVec > 0) );
    }

    bool  computeEigVec(void) const { 
      if( ! initedP )
	notInited();
      return ( (!load_eigenP) && (NWilsVec > 0));
    }

    const EigenIO_t& getEigenIO(void) const {
      if( ! initedP)
	notInited();

      if( ! loadEigVec() )
	notLoadEig();

      return eigen_io; 
    }

    const RitzParams_t& getRitzParams(void) const {
      if (! initedP)
	notInited();

      if( computeEigVec() )
	notComputeEig();

      return ritzery; 
    }

    // Virtual Destructor
    ~ZolotarevStateInfo(void) {}
  };

  void read(XMLReader& xml_in, const std::string& path, ZolotarevStateInfo& info);
  void write(XMLWriter& xml_out, const std::string& path, const ZolotarevStateInfo& info);

};

using namespace Chroma;

#endif
