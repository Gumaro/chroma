// $Id: gauge_init_aggregate.cc,v 3.1 2007-02-04 22:06:42 edwards Exp $
/*! \file
 * @brief Gauge initialization
 */

#include "util/gauge/gauge_init_aggregate.h"

#include "util/gauge/milc_gauge_init.h"
#include "util/gauge/nersc_gauge_init.h"
#include "util/gauge/szinqio_gauge_init.h"
#include "util/gauge/kyu_gauge_init.h"
#include "util/gauge/disordered_gauge_init.h"
#include "util/gauge/unit_gauge_init.h"
#include "util/gauge/cppacs_gauge_init.h"
#include "util/gauge/weak_gauge_init.h"
#include "util/gauge/sf_gauge_init.h"

namespace Chroma
{

  // Registration aggregator
  namespace GaugeInitEnv
  {
    //! Local registration flag
    static bool registered = false;

    //! Register all the factories
    bool registerAll() 
    {
      bool success = true; 
      if (! registered)
      {
	// gauge initialization
	success &= MILCGaugeInitEnv::registerAll();
	success &= NERSCGaugeInitEnv::registerAll();
	success &= SZINQIOGaugeInitEnv::registerAll();
	success &= KYUGaugeInitEnv::registerAll();
	success &= DisorderedGaugeInitEnv::registerAll();
	success &= UnitGaugeInitEnv::registerAll();
	success &= CPPACSGaugeInitEnv::registerAll();
	success &= WeakGaugeInitEnv::registerAll();
	success &= SFGaugeInitEnv::registerAll();

	registered = true;
      }
      return success;
    }
  }

}
