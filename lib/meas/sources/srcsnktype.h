// -*- C++ -*-
// $Id: srcsnktype.h,v 1.1 2004-01-13 03:59:32 edwards Exp $
/*! \file
 *  \brief Source and sink types
 */

#ifndef __srcsnktype_h__
#define __srcsnktype_h__

enum SourceType
{
  SRC_TYPE_POINT_SOURCE, 
  SRC_TYPE_WALL_SOURCE, 
  SRC_TYPE_SHELL_SOURCE, 
  SRC_TYPE_BNDST_SOURCE, 
  SRC_TYPE_POINT_AND_BNDST_SOURCE, 
  SRC_TYPE_SHELL_AND_BNDST_SOURCE, 
  SRC_TYPE_POINT_AND_SHELL_AND_BNDST_SOURCE
};

enum SinkType
{
  SNK_TYPE_POINT_SINK, 
  SNK_TYPE_WALL_SINK, 
  SNK_TYPE_POINT_AND_WALL_SINK, 
  SNK_TYPE_SHELL_SINK, 
  SNK_TYPE_POINT_AND_SHELL_SINK, 
  SNK_TYPE_BNDST_SINK, 
  SNK_TYPE_POINT_AND_BNDST_SINK, 
  SNK_TYPE_SHELL_AND_BNDST_SINK, 
  SNK_TYPE_POINT_AND_SHELL_AND_BNDST_SINK
};

#endif
