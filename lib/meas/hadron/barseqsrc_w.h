// -*- C++ -*-
// $Id: barseqsrc_w.h,v 3.4 2006-10-10 21:01:08 edwards Exp $
/*! \file
 *  \brief Construct baryon sequential sources.
 */

#ifndef __barseqsrc_w_h__
#define __barseqsrc_w_h__

#include "meas/hadron/hadron_seqsource.h"

namespace Chroma 
{

  //! Name and registration
  /*! @ingroup hadron */
  namespace SimpleBaryonSeqSourceEnv
  {
    bool registerAll();

  
    //! Simple baryon sequential source parameters
    /*! @ingroup hadron */
    struct Params
    {
      Params();
      Params(XMLReader& in, const std::string& path);
      void writeXML(XMLWriter& in, const std::string& path) const;

      multi1d<int>     sink_mom;        /*!< sink momentum */
      int              t_sink;          /*!< time slice of sink */
      int              j_decay;         /*!< Decay direction */
    };


    //! Baryon-Baryon seqsources have a time order phase
    /*! @ingroup hadron */
    class BaryonSeqSourceBase : public HadronSeqSource<LatticePropagator>
    {
    public:
      //! Virtual destructor to help with cleanup;
      virtual ~BaryonSeqSourceBase() {}

    protected:
      //! Combine projection with time-ordering
      virtual LatticePropagator projectBaryon(const LatticePropagator& src_prop_tmp,
					      const multi1d<ForwardProp_t>& forward_headers,
					      const multi1d<int>& sink_mom, 
					      int t_sink, int j_decay) const;

      //! Time-ordering phase of source and sink hadron states
      virtual Complex timeOrder(int t_source, int t_sink, int bc_spec) const;
    };



    //! Nucleon-Nucleon U piece with general projector and Cg5
    /*! @ingroup hadron
     *
     * Create a simple baryon sequential propagator source
     */
    class BarNuclUTCg5 : public BaryonSeqSourceBase
    {
    public:
      //! Full constructor
      BarNuclUTCg5(const Params& p, const SpinMatrix& spinT, const SpinMatrix& spinCg5) :
	params(p), T(spinT), Cg5(spinCg5) {}

      //! Default destructor
      ~BarNuclUTCg5() {}
      
      //! Construct the source
      LatticePropagator operator()(const multi1d<LatticeColorMatrix>& u,
				   const multi1d<ForwardProp_t>& forward_headers,
				   const multi1d<LatticePropagator>& forward_props) const;

    private:
      //! Hide partial constructor
      BarNuclUTCg5() {}

    private:
      Params  params;   /*!< Seqsource params */
      SpinMatrix T;     /*!< The spin projector matrix */
      SpinMatrix Cg5;   /*!< The Cg5 at the source and sink */
    };


    //! Nucleon-Nucleon D piece with general projector and Cg5
    /*! @ingroup hadron
     *
     * Create a simple baryon sequential propagator source
     */
    class BarNuclDTCg5 : public BaryonSeqSourceBase
    {
    public:
      //! Full constructor
      BarNuclDTCg5(const Params& p, const SpinMatrix& spinT, const SpinMatrix& spinCg5) :
	params(p), T(spinT), Cg5(spinCg5) {}

      //! Default destructor
      ~BarNuclDTCg5() {}
      
      //! Construct the source
      LatticePropagator operator()(const multi1d<LatticeColorMatrix>& u,
				   const multi1d<ForwardProp_t>& forward_headers,
				   const multi1d<LatticePropagator>& forward_props) const;

    private:
      //! Hide partial constructor
      BarNuclDTCg5() {}

    private:
      Params  params;   /*!< Seqsource params */
      SpinMatrix T;     /*!< The spin projector matrix */
      SpinMatrix Cg5;   /*!< The Cg5 at the source and sink */
    };


    //! Patch for the quarkContract12 piece in NuclUMixedNR and NuclDMixedNR
    /*! @ingroup hadron
     *
     * Create a simple baryon sequential propagator source
     */
    class BarNuclPatchMixedNR : public BaryonSeqSourceBase
    {
    public:
      //! Full constructor
      BarNuclPatchMixedNR(const Params& p) : params(p) {}

      //! Default destructor
      ~BarNuclPatchMixedNR() {}
      
      //! Construct the source
      LatticePropagator operator()(const multi1d<LatticeColorMatrix>& u,
				   const multi1d<ForwardProp_t>& forward_headers,
				   const multi1d<LatticePropagator>& forward_props) const;

    private:
      //! Hide partial constructor
      BarNuclPatchMixedNR() {}

    private:
      Params  params;   /*!< Seqsource params */
    };


    //! Delta+ - Delta+ U piece with general projector and spin matrix
    /*! @ingroup hadron
     *
     * Create a simple baryon sequential propagator source
     */
    class BarDeltaUTsp : public BaryonSeqSourceBase
    {
    public:
      //! Full constructor
      BarDeltaUTsp(const Params& p, const SpinMatrix& spinT, const SpinMatrix& spinSP) :
	params(p), T(spinT), sp(spinSP) {}

      //! Default destructor
      ~BarDeltaUTsp() {}
      
      //! Construct the source
      LatticePropagator operator()(const multi1d<LatticeColorMatrix>& u,
				   const multi1d<ForwardProp_t>& forward_headers,
				   const multi1d<LatticePropagator>& forward_props) const;

    private:
      //! Hide partial constructor
      BarDeltaUTsp() {}

    private:
      Params  params;   /*!< Seqsource params */
      SpinMatrix T;     /*!< The spin projector matrix */
      SpinMatrix sp;    /*!< The spin at the source and sink */
    };

    //! Delta+ - Delta+ D piece with general projector and spin matrix
    /*! @ingroup hadron
     *
     * Create a simple baryon sequential propagator source
     */
    class BarDeltaDTsp : public BaryonSeqSourceBase
    {
    public:
      //! Full constructor
      BarDeltaDTsp(const Params& p, const SpinMatrix& spinT, const SpinMatrix& spinSP) :
	params(p), T(spinT), sp(spinSP) {}

      //! Default destructor
      ~BarDeltaDTsp() {}
      
      //! Construct the source
      LatticePropagator operator()(const multi1d<LatticeColorMatrix>& u,
				   const multi1d<ForwardProp_t>& forward_headers,
				   const multi1d<LatticePropagator>& forward_props) const;

    private:
      //! Hide partial constructor
      BarDeltaDTsp() {}

    private:
      Params  params;   /*!< Seqsource params */
      SpinMatrix T;     /*!< The spin projector matrix */
      SpinMatrix sp;    /*!< The spin at the source and sink */
    };

  }  // end namespace


  //! Reader
  /*! @ingroup hadron */
  void read(XMLReader& xml, const string& path, SimpleBaryonSeqSourceEnv::Params& param);

  //! Writer
  /*! @ingroup hadron */
  void write(XMLWriter& xml, const string& path, const SimpleBaryonSeqSourceEnv::Params& param);

}  // end namespace Chroma


#endif
