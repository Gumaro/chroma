// $Id: gauge_createstate_aggregate.cc,v 3.3 2006-09-07 04:24:28 edwards Exp $
/*! \file
 *  \brief All gauge create-state method
 */

#include "chromabase.h"

#include "actions/gauge/gaugeacts/gauge_createstate_aggregate.h"
#include "actions/gauge/gaugeacts/gauge_createstate_factory.h"

#include "actions/gauge/gaugeacts/simple_gaugestate.h"
#include "actions/gauge/gaugeacts/stout_gaugestate.h"

#include "actions/gauge/gaugebcs/gaugebc_aggregate.h"

namespace Chroma
{

  //! Registration aggregator
  namespace CreateGaugeStateEnv
  {
    bool registerAll() 
    {
      bool success = true;

      // Register all gauge BCs
      success &= GaugeTypeGaugeBCEnv::registered;

      // Register all gauge states
      success &= CreateSimpleGaugeStateEnv::registered;
      success &= CreateStoutGaugeStateEnv::registered;

      return success;
    }

    const bool registered = registerAll();



    // Helper function for the GaugeAction readers
    Handle< CreateGaugeState< multi1d<LatticeColorMatrix>, 
			      multi1d<LatticeColorMatrix> > > reader(XMLReader& xml_in, 
								     const std::string& path)
    {
      XMLReader top(xml_in, path);

      std::string gaugestate;
      std::string gaugestate_path;
      if (top.count("GaugeState") != 0)
      {
	gaugestate_path = "GaugeState";
	read(top, gaugestate_path + "/Name", gaugestate);
      }
      else
      {
//	QDPIO::cerr << "Error: GaugeState group not found" << endl;
//	QDP_abort(1);
	
	gaugestate_path = ".";
	gaugestate = Chroma::CreateSimpleGaugeStateEnv::name;
      }

      Handle< CreateGaugeState< multi1d<LatticeColorMatrix>, multi1d<LatticeColorMatrix> > > 
	cgs(TheCreateGaugeStateFactory::Instance().createObject(gaugestate,
								top,
								gaugestate_path));

      return cgs;
    }

  }
}
