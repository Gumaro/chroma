#include "chromabase.h"
#include "io/param_io.h"

using namespace QDP;
using namespace std;
using namespace Chroma;

Zolotarev4DFermActParams::Zolotarev4DFermActParams(XMLReader& in)
{

  // This will bomb if it fails so no need to check for NULL after
  AuxFermActHandle = read(in, "AuxFermAct");
  
  try { 

    read(in, "Mass", Mass);
    read(in, "RatPolyDeg", RatPolyDeg);


    if(in.count("RatPolyDegPrecond") == 1 ) { 
      read(in, "RatPolyDegPrecond", RatPolyDegPrecond);
    }
    else {
      RatPolyDegPrecond = RatPolyDeg;
    }

    read(in, "InnerSolve/MaxCG", MaxCGInner);
    read(in, "InnerSolve/RsdCG", RsdCGInner);
    if( in.count("InnerSolve/ReorthFreq") == 1 ) {
      read(in, "InnerSolve/ReorthFreq", ReorthFreqInner);
    }
    else {
      ReorthFreqInner = 10; // Some default
    }

    if( in.count("InnerSolve/SolverType") == 1 ) { 
      read(in, "InnerSolve/SolverType", InnerSolverType);
    }
    else {
      InnerSolverType = OVERLAP_INNER_CG_SINGLE_PASS;
    }

    read(in, "StateInfo", StateInfo);
  }
  catch( const string &e ) {
    QDPIO::cerr << "Caught Exception reading Zolo4D Fermact params: " << e << endl;
    QDP_abort(1);
  }
}

void write(XMLWriter& xml_out, const string& path, const Zolotarev4DFermActParams& p)
{
  if ( path != "." ) { 
    push( xml_out, path);
  }
  
  write(xml_out, "FermAct", p.getFermActType());

  switch(p.AuxFermActHandle->getFermActType()) {
  case FERM_ACT_WILSON:
    {
      const WilsonFermActParams& wilson = dynamic_cast<WilsonFermActParams&>(*(p.AuxFermActHandle));
      write(xml_out, "AuxFermAct", wilson);
    }
    break;
  default:
    QDPIO::cerr << "Unsupported AuxFermAct in write " << endl;
    QDP_abort(1);
    break;
  }

  write(xml_out, "Mass", p.Mass);
  write(xml_out, "RatPolyDeg", p.RatPolyDeg);
  write(xml_out, "RatPolyDegPrecond", p.RatPolyDegPrecond);
  push(xml_out, "InnerSolve");
  write(xml_out, "MaxCG", p.MaxCGInner);
  write(xml_out, "RsdCG", p.RsdCGInner);
  write(xml_out, "ReorthFreq", p.ReorthFreqInner);
  write(xml_out, "SolverType", p.InnerSolverType);
  pop(xml_out);

  write(xml_out, "StateInfo", p.StateInfo);

  if( path != "." ) { 
    pop(xml_out);
  }
}

