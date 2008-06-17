// $Id: qio_read_obj_funcmap.cc,v 3.6 2008-06-17 15:36:58 edwards Exp $
/*! \file
 *  \brief Read object function map
 */

#include "named_obj.h"
#include "meas/inline/io/qio_read_obj_funcmap.h"
#include "meas/inline/io/named_objmap.h"

#include "meas/hadron/diquark_w.h"
#include "util/ferm/eigeninfo.h"
#include "actions/ferm/invert/containers.h"

namespace Chroma
{
 
  //! IO function map environment
  /*! \ingroup inlineio */
  namespace QIOReadObjCallMapEnv
  { 
    // Anonymous namespace
    namespace
    {
      //------------------------------------------------------------------------
      //! Read a propagator
      void QIOReadLatProp(const string& buffer_id,
			  const string& file, 
			  QDP_serialparallel_t serpar)
      {
	LatticePropagator obj;
	XMLReader file_xml, record_xml;

	QDPFileReader to(file_xml,file,serpar);
	read(to,record_xml,obj);
	close(to);

	TheNamedObjMap::Instance().create<LatticePropagator>(buffer_id);
	TheNamedObjMap::Instance().getData<LatticePropagator>(buffer_id) = obj;
	TheNamedObjMap::Instance().get(buffer_id).setFileXML(file_xml);
	TheNamedObjMap::Instance().get(buffer_id).setRecordXML(record_xml);
      }


      //! Read a single prec propagator
      void QIOReadLatPropF(const string& buffer_id,
			   const string& file, 
			   QDP_serialparallel_t serpar)
      {
	LatticePropagatorF obj;
	XMLReader file_xml, record_xml;

	QDPFileReader to(file_xml,file,serpar);
	read(to,record_xml,obj);
	close(to);

	TheNamedObjMap::Instance().create<LatticePropagator>(buffer_id);
	TheNamedObjMap::Instance().getData<LatticePropagator>(buffer_id) = obj;
	TheNamedObjMap::Instance().get(buffer_id).setFileXML(file_xml);
	TheNamedObjMap::Instance().get(buffer_id).setRecordXML(record_xml);
      }


      //! Read a double prec propagator
      void QIOReadLatPropD(const string& buffer_id,
			   const string& file, 
			   QDP_serialparallel_t serpar)
      {
	LatticePropagatorD obj;
	XMLReader file_xml, record_xml;

	QDPFileReader to(file_xml,file,serpar);
	read(to,record_xml,obj);
	close(to);

	TheNamedObjMap::Instance().create<LatticePropagator>(buffer_id);
	TheNamedObjMap::Instance().getData<LatticePropagator>(buffer_id) = obj;
	TheNamedObjMap::Instance().get(buffer_id).setFileXML(file_xml);
	TheNamedObjMap::Instance().get(buffer_id).setRecordXML(record_xml);
      }



      //------------------------------------------------------------------------
      //! Read a fermion
      void QIOReadLatFerm(const string& buffer_id,
			  const string& file, 
			  QDP_serialparallel_t serpar)
      {
	LatticeFermion obj;
	XMLReader file_xml, record_xml;

	QDPFileReader to(file_xml,file,serpar);
	read(to,record_xml,obj);
	close(to);

	TheNamedObjMap::Instance().create<LatticeFermion>(buffer_id);
	TheNamedObjMap::Instance().getData<LatticeFermion>(buffer_id) = obj;
	TheNamedObjMap::Instance().get(buffer_id).setFileXML(file_xml);
	TheNamedObjMap::Instance().get(buffer_id).setRecordXML(record_xml);
      }


#if 0
      // RGE: FOR SOME REASON, QDP CANNOT CAST A DOUBLE TO FLOATING HERE. NEED TO FIX.

      //! Read a single prec fermion
      void QIOReadLatFermF(const string& buffer_id,
			   const string& file, 
			   QDP_serialparallel_t serpar)
      {
	LatticeFermionF obj;
	XMLReader file_xml, record_xml;

	QDPFileReader to(file_xml,file,serpar);
	read(to,record_xml,obj);
	close(to);

	TheNamedObjMap::Instance().create<LatticeFermion>(buffer_id);
	TheNamedObjMap::Instance().getData<LatticeFermion>(buffer_id) = obj;
	TheNamedObjMap::Instance().get(buffer_id).setFileXML(file_xml);
	TheNamedObjMap::Instance().get(buffer_id).setRecordXML(record_xml);
      }


      //! Read a double prec fermion
      void QIOReadLatFermD(const string& buffer_id,
			   const string& file, 
			   QDP_serialparallel_t serpar)
      {
	LatticeFermionD obj;
	XMLReader file_xml, record_xml;

	QDPFileReader to(file_xml,file,serpar);
	read(to,record_xml,obj);
	close(to);

	TheNamedObjMap::Instance().create<LatticeFermion>(buffer_id);
	TheNamedObjMap::Instance().getData<LatticeFermion>(buffer_id) = obj;
	TheNamedObjMap::Instance().get(buffer_id).setFileXML(file_xml);
	TheNamedObjMap::Instance().get(buffer_id).setRecordXML(record_xml);
      }
#endif

      //------------------------------------------------------------------------
      //! Read a gauge field in floating precision
      void QIOReadArrayLatColMat(const string& buffer_id,
				 const string& file, 
				 QDP_serialparallel_t serpar)
      {
	multi1d<LatticeColorMatrix> obj;
	XMLReader file_xml, record_xml;

	obj.resize(Nd);    // BAD BAD BAD - FIX THIS

	QDPFileReader to(file_xml,file,serpar);
	read(to,record_xml,obj);
	close(to);

	TheNamedObjMap::Instance().create< multi1d<LatticeColorMatrix> >(buffer_id);
	TheNamedObjMap::Instance().getData< multi1d<LatticeColorMatrix> >(buffer_id) = obj;
	TheNamedObjMap::Instance().get(buffer_id).setFileXML(file_xml);
	TheNamedObjMap::Instance().get(buffer_id).setRecordXML(record_xml);
      }

      //! Read a single prec Gauge fields
      void QIOReadArrayLatColMatF(const string& buffer_id,
				  const string& file, 
				  QDP_serialparallel_t serpar)
      {
	multi1d<LatticeColorMatrixF> obj;
	XMLReader file_xml, record_xml;

	obj.resize(Nd);    // BAD BAD BAD - FIX THIS

	QDPFileReader to(file_xml,file,serpar);
	read(to,record_xml,obj);
	close(to);

	TheNamedObjMap::Instance().create< multi1d<LatticeColorMatrix> >(buffer_id);
	(TheNamedObjMap::Instance().getData< multi1d<LatticeColorMatrix> >(buffer_id)).resize(Nd);
	for(int i=0; i < Nd; i++) {
	    (TheNamedObjMap::Instance().getData< multi1d<LatticeColorMatrix> >(buffer_id))[i] = obj[i];
	}
	TheNamedObjMap::Instance().get(buffer_id).setFileXML(file_xml);
	TheNamedObjMap::Instance().get(buffer_id).setRecordXML(record_xml);
      }

      //! Read a Double Prec Gauge Field
      void QIOReadArrayLatColMatD(const string& buffer_id,
				  const string& file, 
				  QDP_serialparallel_t serpar)
      {
	multi1d<LatticeColorMatrixD> obj;
	XMLReader file_xml, record_xml;

	obj.resize(Nd);    // BAD BAD BAD - FIX THIS

	QDPFileReader to(file_xml,file,serpar);
	read(to,record_xml,obj);
	close(to);

	TheNamedObjMap::Instance().create< multi1d<LatticeColorMatrix> >(buffer_id);
	(TheNamedObjMap::Instance().getData< multi1d<LatticeColorMatrix> >(buffer_id)).resize(Nd);
	for(int i=0; i < Nd;i++) {
	    (TheNamedObjMap::Instance().getData< multi1d<LatticeColorMatrix> >(buffer_id))[i] = obj[i];
	}

	TheNamedObjMap::Instance().get(buffer_id).setFileXML(file_xml);
	TheNamedObjMap::Instance().get(buffer_id).setRecordXML(record_xml);
      }


      //------------------------------------------------------------------------
      //! Read a QQDiquark object in floating precision
      void QIOReadQQDiquarkContract(const string& buffer_id,
				    const string& file, 
				    QDP_serialparallel_t serpar)
      {
	// Read the 1-d flattened array version
	XMLReader file_xml, record_xml;
	const int sz_qq = Ns*Ns*Ns*Ns*Nc*Nc;
	multi1d<LatticeComplex> obj_1d(sz_qq);

	QDPFileReader to(file_xml,file,serpar);
	read(to,record_xml,obj_1d);
	close(to);

	// Create the named object
	TheNamedObjMap::Instance().create<QQDiquarkContract_t>(buffer_id);
	TheNamedObjMap::Instance().get(buffer_id).setFileXML(file_xml);
	TheNamedObjMap::Instance().get(buffer_id).setRecordXML(record_xml);

	// Convert the 1-d object into an N-d object
	QQDiquarkContract_t& obj = TheNamedObjMap::Instance().getData<QQDiquarkContract_t>(buffer_id);
	multi1d<int> sz(6);
	sz = Ns;
	sz[4] = Nc;  // cf
	sz[5] = Nc;  // ci
	obj.comp.resize(sz);

	int cnt = 0;
	multi1d<int> ranks(6);
	for(ranks[0]=0; ranks[0] < sz[0]; ++ranks[0])
	  for(ranks[1]=0; ranks[1] < sz[1]; ++ranks[1])
	    for(ranks[2]=0; ranks[2] < sz[2]; ++ranks[2])
	      for(ranks[3]=0; ranks[3] < sz[3]; ++ranks[3])
		for(ranks[4]=0; ranks[4] < sz[4]; ++ranks[4])
		  for(ranks[5]=0; ranks[5] < sz[5]; ++ranks[5])
		  {
		    // Sanity check - the size better match
		    if (cnt >= sz_qq)
		    {
		      QDPIO::cerr << __func__ << ": size mismatch for multi1Nd object" << endl;
		      QDP_abort(1);
		    }

		    obj.comp[ranks] = obj_1d[cnt++];
		  }

      }


      //------------------------------------------------------------------------
      template<typename T>
      void QIOReadEigenInfo_a(const string& buffer_id,
			      const string& file,
			      QDP_serialparallel_t serpar)
      {
	multi1d<Real64> evalsD;

	// RGE: I BELIEVE ALL THE COMPLAINTS BELOW ARE NOW FIXED IN QDP++,
	// BUT I DO NOT WANT TO REMOVE THE CODE YET - CANNOT BE BOTHERED DEBUGGING

	// BUG? Need to read these as an array even though there is only one
	// and also I need to know in advance the array size.
	multi1d<Real64> largestD(1);

	// Would like this to be double, but casting double prec to single prec doesn't work.
	T evecD;

	// Stuff 
	XMLReader file_xml, record_xml, largest_record_xml;

	// Open file
	QDPFileReader to(file_xml,file,serpar);

	// Create object
	TheNamedObjMap::Instance().create< EigenInfo<T> >(buffer_id);

	// Read largest ev plus XML file containing number of eval/evec pairs
	read(to, largest_record_xml, largestD);

	// Extract number of EVs from XML
	int size;
	try { 
	  read(largest_record_xml, "/NumElem/n_vec", size);
	}
	catch(const std::string& e) { 
	  QDPIO::cerr<< "Caught Exception while reading XML: " << e << endl;
	  QDP_abort(1);
	}

	// Set largest EV
	TheNamedObjMap::Instance().getData< EigenInfo<T> >(buffer_id).getLargest()=largestD[0];

	// REsize eval arrays so that IO works correctly
	evalsD.resize(size);

	// Read evals
	read(to, record_xml, evalsD);

	QDPIO::cout << "Read " << evalsD.size() << "Evals " << endl;
	for(int i=0; i < evalsD.size(); i++) { 
	  QDPIO::cout << "Eval["<<i<<"] = " << Real(evalsD[i]) << endl;
	}

	// Resize eval array 
	TheNamedObjMap::Instance().getData< EigenInfo<T> >(buffer_id).getEvalues().resize(evalsD.size());
      
	// Downcast evals to Real() precision
	for (int i=0; i<evalsD.size(); i++)
	  TheNamedObjMap::Instance().getData< EigenInfo<T> >(buffer_id).getEvalues()[i] = Real(evalsD[i]);


	// Resize evec arrays
	TheNamedObjMap::Instance().getData< EigenInfo<T> >(buffer_id).getEvectors().resize(evalsD.size());

	// Loop and read evecs
	for (int i=0; i<evalsD.size(); i++)
	{
	  XMLReader record_xml_dummy;
	  read(to, record_xml_dummy, evecD);
	  T evecR;
	  evecR=evecD;

	  (TheNamedObjMap::Instance().getData< EigenInfo<T> >(buffer_id).getEvectors())[i]=evecR;
	}
 
	// Set File and Record XML throw away dummy XMLs
	TheNamedObjMap::Instance().get(buffer_id).setFileXML(file_xml);
	TheNamedObjMap::Instance().get(buffer_id).setRecordXML(record_xml);

	// Done - That too was unnecessarily painful
	close(to);
      }

      //------------------------------------------------------------------------
      void QIOReadEigenInfoLatticeFermion(const string& buffer_id,
					  const string& file,
					  QDP_serialparallel_t serpar)
      {
	QIOReadEigenInfo_a<LatticeFermion>(buffer_id, file, serpar);
      }

      //------------------------------------------------------------------------
      void QIOReadEigenInfoLatticeColorVector(const string& buffer_id,
					      const string& file,
					      QDP_serialparallel_t serpar)
      {
	QIOReadEigenInfo_a<LatticeColorVector>(buffer_id, file, serpar);
      }

      //------------------------------------------------------------------------
      //! Write out an RitzPairs Type
      void QIOReadRitzPairsLatticeFermion(const string& buffer_id,
					  const string& file,
					  QDP_serialparallel_t serpar)
      {
	// File XML
	XMLReader file_xml;

	// Open file
	QDPFileReader to(file_xml,file,serpar);

	// Create the named object
	TheNamedObjMap::Instance().create< LinAlg::RitzPairs<LatticeFermion> >(buffer_id);
	TheNamedObjMap::Instance().get(buffer_id).setFileXML(file_xml);

	// A shorthand for the object
	LinAlg::RitzPairs<LatticeFermion>& obj = 
	  TheNamedObjMap::Instance().getData<LinAlg::RitzPairs< LatticeFermion> >(buffer_id);

	XMLReader record_xml;
	TheNamedObjMap::Instance().get(buffer_id).setRecordXML(record_xml);
	
	int Nmax;
	read(file_xml, "/RitzPairs/Nmax", Nmax);
	read(file_xml, "/RitzPairs/Neig", obj.Neig);

	obj.evec.resize(Nmax);
	obj.eval.resize(Nmax);

	if (obj.Neig > Nmax)
	{
	  QDPIO::cerr << __func__ << ": error, found Neig > Nmax" << endl;
	  QDP_abort(1);
	}

	// Read a record for each eigenvalue (in xml) and eigenvector
	for(int i=0; i < obj.Neig; ++i)
	{
	  XMLReader record_xml;
	  read(to, record_xml, obj.evec.vec[i]);

	  read(record_xml, "/Eigenvector/eigenValue", obj.eval.vec[i]);
	}

	// Close
	close(to);
      }

      //! Local registration flag
      bool registered = false;

    }  // end namespace


    //! Register all the factories
    bool registerAll() 
    {
      bool success = true; 
      if (! registered)
      {
	success &= TheQIOReadObjFuncMap::Instance().registerFunction(string("LatticePropagator"), 
								     QIOReadLatProp);
	success &= TheQIOReadObjFuncMap::Instance().registerFunction(string("LatticePropagatorF"), 
								     QIOReadLatPropF);
	success &= TheQIOReadObjFuncMap::Instance().registerFunction(string("LatticePropagatorD"), 
								     QIOReadLatPropD);

	success &= TheQIOReadObjFuncMap::Instance().registerFunction(string("LatticeFermion"), 
								   QIOReadLatFerm);
//      success &= TheQIOReadObjFuncMap::Instance().registerFunction(string("LatticeFermionF"), 
//								   QIOReadLatFermF);
//      success &= TheQIOReadObjFuncMap::Instance().registerFunction(string("LatticeFermionD"), 
//								   QIOReadLatFermD);

	success &= TheQIOReadObjFuncMap::Instance().registerFunction(string("Multi1dLatticeColorMatrix"), 
								     QIOReadArrayLatColMat);

	success &= TheQIOReadObjFuncMap::Instance().registerFunction(string("Multi1dLatticeColorMatrixF"), 
								     QIOReadArrayLatColMatF);

	success &= TheQIOReadObjFuncMap::Instance().registerFunction(string("Multi1dLatticeColorMatrixD"), 
								     QIOReadArrayLatColMatD);

	success &= TheQIOReadObjFuncMap::Instance().registerFunction(string("QQDiquarkContract"), 
								     QIOReadQQDiquarkContract);
	
	success &= TheQIOReadObjFuncMap::Instance().registerFunction(string("EigenInfoLatticeFermion"),
								     QIOReadEigenInfoLatticeFermion);

	success &= TheQIOReadObjFuncMap::Instance().registerFunction(string("EigenInfoLatticeColorVector"),
								     QIOReadEigenInfoLatticeColorVector);

	success &= TheQIOReadObjFuncMap::Instance().registerFunction(string("RitzPairsLatticeFermion"), 
								     QIOReadRitzPairsLatticeFermion);
	
	registered = true;
      }
      return success;
    }
  }

}
