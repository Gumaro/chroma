// -*- C++ -*-
// $Id: dilution_quark_source_const_w.h,v 1.8 2008-01-17 21:49:11 jbulava Exp $
/*! \file
 * \brief Dilution scheme inferred from pre-generated solutions.
 * 
 * This dilution scheme takes pre-generated LatticeFermion solutions built
 * in the chain of MAKE_SOURCE and PROPAGATOR inline measurement calls
 */

#ifndef __dilution_quark_source_const_h__
#define __dilution_quark_source_const_h__

#include "chromabase.h"
#include "meas/inline/abs_inline_measurement.h"
#include "meas/hadron/dilution_scheme.h"
#include "io/qprop_io.h"

namespace Chroma 
{ 
  /*! \ingroup inlinehadron */
  namespace DilutionQuarkSourceConstEnv 
  {
    extern const std::string name;
    bool registerAll();

    //! Parameter structure
    /*! \ingroup inlinehadron */
    struct Params
    {
      Params();
      Params(XMLReader& xml_in, const std::string& path);
      void writeXML(XMLWriter& xml_out, const std::string& path) const;

    		struct QuarkFiles_t
      {
	   		struct TimeSliceFiles_t 
				{
				
					multi1d<std::string> dilution_files;  /*!< dilution files per timeslice*/		
				};

					
				multi1d<TimeSliceFiles_t> timeslice_files; /*!< full time dilution is assumed*/ 
			
			};
	
      QuarkFiles_t  quark_files;       /*!< All the solutions for a single quark */
    }; // struct Params

  //! Structure holding a source and its solutions
    struct QuarkSourceSolutions_t
    {
			struct TimeSlices_t
			{


				struct Dilutions_t
				{
				std::string        soln_file;  //Solution filename for this dilution 
	  		PropSourceConst_t  source_header;
	  		ChromaProp_t       prop_header;
				};
	
				
				int       t0;      //time slice on which this diluted source 
													 //has support. Full time dilution is assumed
			
				multi1d<Dilutions_t>  dilutions; 
      
			};
				
			multi1d<TimeSlices_t> timeslices;

			int   decay_dir;
      Seed  seed;
    
		};
      
		

    //! Dilution scheme constructed by propagator solutions over diluted MAKE_SOURCE calls 
    class ConstDilutionScheme : public DilutionScheme<LatticeFermion>
    {
    public:

      //! Virtual destructor to help with cleanup;
      ~ConstDilutionScheme() {}

      //! The decay direction
      int getDecayDir() const {return quark.decay_dir;}

      //! The seed identifies this quark
      const Seed& getSeed() const {return quark.seed;}

      //! The actual t0 corresponding to this time dilution element 
      int getT0( int t0 ) const {return quark.timeslices[t0].t0;}

      //! The number of dilution timeslices included  
      int getNumTimeSlices() const {return quark.timeslices.size();}

      //! Return the diluted source vector
      LatticeFermion dilutedSource(int t0, int dil) const;
    
      //! Return the solution vector corresponding to the diluted source
      LatticeFermion dilutedSolution(int t0, int dil) const;

    protected:
      //! Initialize the object
      void init();

    private:
      //! Hide default constructor
      ConstDilutionScheme( const Params& p ) 
			{ 
				
				params = p; 
				init();

			}

    private:
      Params params;
      QuarkSourceSolutions_t quark;
    };
    
  } // namespace DilutionQuarkSourceConstEnv


  //! Reader
  /*! @ingroup hadron */
  void read(XMLReader& xml, const string& path, DilutionQuarkSourceConstEnv::Params& param);

  //! Writer
  /*! @ingroup hadron */
  void write(XMLWriter& xml, const string& path, const DilutionQuarkSourceConstEnv::Params& param);

} // namespace Chroma

#endif
