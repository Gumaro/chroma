// $Id: ferm_createstate_reader_s.cc,v 3.1 2006-08-18 15:51:55 edwards Exp $
/*! \file
 *  \brief All ferm create-state method
 */

#include "chromabase.h"

#include "actions/ferm/fermacts/simple_fermstate.h"
#include "actions/ferm/fermacts/ferm_createstate_factory_s.h"
#include "actions/ferm/fermacts/ferm_createstate_reader_s.h"
#include "actions/ferm/fermacts/ferm_createstate_aggregate_s.h"

namespace Chroma
{

  //! Registration aggregator
  namespace StaggeredCreateFermStateEnv
  {

    // Helper function for the FermAction readers
    Handle< CreateFermState<LatticeStaggeredFermion,
			    multi1d<LatticeColorMatrix>, 
			    multi1d<LatticeColorMatrix> > > reader(XMLReader& xml_in, 
								   const std::string& path)
    {
      XMLReader top(xml_in, path);

      bool success = registered;  // make sure all codes loaded

      std::string fermstate;
      std::string fermstate_path;
      if (top.count("FermState") != 0)
      {
	fermstate_path = "FermState";
	read(top, fermstate_path + "/Name", fermstate);
      }
      else
      {
	QDPIO::cerr << "Error: FermState group not found" << endl;
	QDP_abort(1);
      }

      Handle< CreateFermState<
	LatticeStaggeredFermion, multi1d<LatticeColorMatrix>, multi1d<LatticeColorMatrix> > > 
	cgs(TheStaggeredCreateFermStateFactory::Instance().createObject(fermstate,
									top,
									fermstate_path));

      return cgs;
    }

  }

}
