#ifndef GLOBAL_METROP_ACCREJ
#define GLOBAL_METROP_ACCREJ

#include "chromabase.h"


namespace Chroma { 
  // This needs to move to a .cc file at some stage
  bool globalMetropolisAcceptReject(const Double& DeltaH);
};
#endif
