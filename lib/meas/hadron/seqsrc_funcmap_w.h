// -*- C++ -*-
// $Id: seqsrc_funcmap_w.h,v 1.1 2005-03-07 02:55:20 edwards Exp $
/*! \file
 *  \brief Sequential source function map
 */

#ifndef __seqsrc_funcmap_h__
#define __seqsrc_funcmap_h__

#include "singleton.h"
#include "funcmap.h"

namespace Chroma
{

  //! Sequential Source function map
  typedef SingletonHolder< 
    FunctionMap<LatticePropagator, 
		std::string,
		TYPELIST_3(const LatticePropagator&, const LatticePropagator&, const LatticePropagator&),
		LatticePropagator (*)(const LatticePropagator&, 
				      const LatticePropagator&, 
				      const LatticePropagator&),
		StringFunctionMapError> >
  TheSeqSourceFuncMap;


  namespace SeqSourceCallMapEnv
  { 
    extern bool registered;   // forward decl
  };

} // end namespace Chroma


#endif
