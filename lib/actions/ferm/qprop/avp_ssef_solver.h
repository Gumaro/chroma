#ifndef AVP_SSEF_SOLVER_H
#define AVP_SSEF_SOLVER_H

#include "avp_inverter_interface.h"

extern "C" {
  struct MIT_ssef_DWF_Gauge;
  struct MIT_ssef_DWF_Fermion;
};

using namespace QDP;
namespace Chroma { 
  namespace AVPSolver { 
    
    class SSEDWFSolverF : public AVPSolverInterface< MIT_ssef_DWF_Gauge, MIT_ssef_DWF_Fermion > {
public:
    protected:
      MIT_ssef_DWF_Fermion* loadFermionRHS(const void* OuterFermion) const;
      MIT_ssef_DWF_Fermion* loadFermionGuess(const void *OuterFermion) const;
      MIT_ssef_DWF_Fermion* allocateFermion(void) const ;
      void saveFermionSolver(void *OuterFermion, 
		       MIT_ssef_DWF_Fermion* CGFermion) const;

     void saveFermionOperator(void *OuterFermion, 
		       MIT_ssef_DWF_Fermion* CGFermion) const;

     void deleteFermion(MIT_ssef_DWF_Fermion* ptr) const;
     int cgInternal(MIT_ssef_DWF_Fermion       *psi,
		    double        *out_eps,
		    int           *out_iter,
		    double        M,
		    double        m_f,
		    const MIT_ssef_DWF_Fermion *x0,
		    const MIT_ssef_DWF_Fermion *eta,
		    double        eps,
		    int           min_iter,
		    int           max_iter)  const;
    public:
     void loadGauge(const void *u,
		    const void *v);
     
     void deleteGauge(void);
     
     // Init the system -- Constructor call?
     int init(const int lattice[5],
	       void *(*allocator)(size_t size),
	       void (*deallocator)(void *));
     
     // Finalize - destructor call
     void fini(void);
    private:
     MIT_ssef_DWF_Gauge *g;
    };
  };
};

#endif
