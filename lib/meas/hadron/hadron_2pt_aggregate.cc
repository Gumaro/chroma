// $Id: hadron_2pt_aggregate.cc,v 1.1 2007-05-09 17:19:44 edwards Exp $
/*! \file
 *  \brief All hadron 2pt constructors
 */

#include "meas/hadron/hadron_2pt_aggregate.h"

#include "meas/hadron/simple_meson_2pt_w.h"
//#include "meas/hadron/simple_baryon_2pt_w.h"
//#include "meas/hadron/deriv_meson_2pt_w.h"

namespace Chroma
{

  //! Registration aggregator
  namespace Hadron2PtEnv
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
	// Hadron
	success &= SimpleMeson2PtEnv::registerAll();
//	success &= SimpleBaryon2PtEnv::registerAll();
//	success &= DerivMeson2PtEnv::registerAll();

	registered = true;
      }
      return success;
    }
  }

}
