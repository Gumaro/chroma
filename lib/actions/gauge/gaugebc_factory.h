// -*- C++ -*-
// $Id: gaugebc_factory.h,v 1.1 2004-12-30 10:29:36 bjoo Exp $
/*! \file
 *  \brief Fermion action factories
 */

#ifndef __gaugebcfactory_h__
#define __gaugebcfactory_h__

#include "singleton.h"
#include "objfactory.h"
#include "chromabase.h"

#include "gaugebc.h"

namespace Chroma
{

  //! GaugeAct Factory 
  typedef SingletonHolder< 
  ObjectFactory<GaugeBC, 
    std::string,
    TYPELIST_2(XMLReader&, const std::string&),
    GaugeBC* (*)(XMLReader&, const std::string&), 
		StringFactoryError> >
  TheGaugeBCFactory;
}; // end namespace Chroma


#endif
