// $Id: source_const_aggregate.cc,v 3.2 2006-10-30 21:59:09 edwards Exp $
/*! \file
 *  \brief All make source constructors
 */

#include "meas/sources/source_const_aggregate.h"

#include "meas/sources/pt_source_const.h"
#include "meas/sources/sh_source_const.h"
#include "meas/sources/wall_source_const.h"
#include "meas/sources/mom_source_const.h"
#include "meas/sources/partwall_source_const.h"

#include "meas/sources/rndz2wall_source_const.h"
#include "meas/sources/dilutezN_source_const.h"

namespace Chroma
{

  //! Registration aggregator
  namespace QuarkSourceConstructionEnv
  {
    //! Local registration flag
    static bool registered = false;

    //! Register all the factories
    bool registerAll() 
    {
      bool success = true; 
      if (! registered)
      {
	// Sources
	success &= PointQuarkSourceConstEnv::registerAll();
	success &= ShellQuarkSourceConstEnv::registerAll();
	success &= RandZ2WallQuarkSourceConstEnv::registerAll();
	success &= WallQuarkSourceConstEnv::registerAll();
//	success &= MomWallQuarkSourceConstEnv::registerAll();
	success &= PartialWallQuarkSourceConstEnv::registerAll();
	success &= DiluteZNQuarkSourceConstEnv::registerAll();

	registered = true;
      }
      return success;
    }
  }

}
