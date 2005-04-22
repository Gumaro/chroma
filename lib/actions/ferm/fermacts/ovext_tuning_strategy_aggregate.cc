#include "ovext_tuning_strategy_aggregate.h"
#include "actions/ferm/fermacts/ovext_constant_strategy.h"
#include "actions/ferm/fermacts/ovext_const_div_by_resp_strategy.h"

namespace Chroma { 
  namespace OvExtTuningStrategyAggregateEnv {
    
    bool registerAll()
    {
      bool success = true;
      success &= OvExtConstantStrategyEnv::registered;
      success &= OvExtConstDivByResPStrategyEnv::registered;
      return success;
    }
    
    const bool registered = registerAll();
  };
}
