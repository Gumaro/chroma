// $Id: inline_hadron_aggregate_s.cc,v 3.2 2006-11-17 02:17:31 edwards Exp $
/*! \file
 *  \brief Inline hadron measurement aggregator
 */

#include "meas/inline/hadron_s/inline_hadron_aggregate_s.h"
#include "meas/inline/hadron_s/inline_spectrum_s.h"
#include "meas/inline/hadron_s/inline_make_source_s.h"
#include "meas/inline/hadron_s/inline_propagator_s.h"
#include "meas/inline/hadron_s/inline_sink_smear_s.h"

namespace Chroma
{

  //! Name and registration
  namespace InlineStaggeredHadronAggregateEnv
  {
    namespace
    {
      //! Local registration flag
      bool registered = false;
    }

    //! Register all the factories
    bool registerAll() 
    {
      bool success = true; 
      if (! registered)
      {
	// Hadron stuff
	success &= InlineStaggeredSpectrumEnv::registerAll();
	success &= InlineStaggeredMakeSourceEnv::registerAll();
	success &= InlineStaggeredPropagatorEnv::registerAll();
	success &= InlineStaggeredSinkSmearEnv::registerAll();
	registered = true;
      }
      return success;
    }
  }

}
