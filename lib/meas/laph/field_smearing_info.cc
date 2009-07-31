#include "field_smearing_info.h"
#include "xml_help.h"
using namespace std;

namespace Chroma {
  namespace LaphEnv {



FieldSmearingInfo::FieldSmearingInfo(const FieldSmearingInfo& in) 
            : linkIterations(in.linkIterations),
              linkStapleWeight(in.linkStapleWeight),
              laphNumEigvecs(in.laphNumEigvecs),
              laphSigma(in.laphSigma) {}

FieldSmearingInfo& FieldSmearingInfo::operator=(const FieldSmearingInfo& in)
{
 linkIterations=in.linkIterations;
 linkStapleWeight=in.linkStapleWeight;
 laphNumEigvecs=in.laphNumEigvecs;
 laphSigma=in.laphSigma;
 return *this;
}

void FieldSmearingInfo::checkEqual(const FieldSmearingInfo& in) const
{
 if  ((linkIterations!=in.linkIterations)
    ||(abs(linkStapleWeight-in.linkStapleWeight)>1e-12)
    ||(abs(laphSigma-in.laphSigma)>1e-12)
    ||(laphNumEigvecs!=in.laphNumEigvecs))
    throw string("FieldSmearingInfo checkEqual failed...");
}

bool FieldSmearingInfo::operator==(const FieldSmearingInfo& in) const
{
 return ((linkIterations==in.linkIterations)
       &&(abs(linkStapleWeight-in.linkStapleWeight)<1e-12)
       &&(abs(laphSigma-in.laphSigma)<1e-12)
       &&(laphNumEigvecs==in.laphNumEigvecs));
}


   // fatally aborting if not found

FieldSmearingInfo::FieldSmearingInfo(XMLReader& xml_rdr)
{
 int link_it,quark_nvecs;
 double link_wt, laph_sigma;
 try{
    XMLReader xml_in(xml_rdr, "//stout_laph_smearing");
    read(xml_in,"./link_iterations", link_it );
    read(xml_in,"./link_staple_weight", link_wt );
    read(xml_in,"./laph_sigma_cutoff", laph_sigma );
    read(xml_in,"./number_laph_eigvecs", quark_nvecs );
    }
 catch(const string& err){
    QDPIO::cerr << "ERROR: "<<err<<endl;
    QDPIO::cerr << "could not initialize FieldSmearingInfo from XML input"<<endl;
    QDP_abort(1);}
 assign(link_it,link_wt,quark_nvecs,laph_sigma);
}

void FieldSmearingInfo::assign(int link_it, double link_wt, int quark_nvecs,
                               double laph_sigma)
{
 linkIterations=link_it;
 linkStapleWeight=link_wt;
 laphNumEigvecs=quark_nvecs;
 laphSigma=laph_sigma;
 if ((linkIterations<0)||(linkStapleWeight<0.0)||(laphNumEigvecs<1)
    ||(laphSigma<=0)){
    QDPIO::cerr << "invalid smearing scheme"<<endl;
    QDP_abort(1);}
}


string FieldSmearingInfo::output(int indent) const
{
 string pad(3*indent,' ');
 ostringstream oss;
 oss << pad << "<stout_laph_smearing>"<<endl;
 oss << pad << "   <link_iterations> " << linkIterations 
     << " </link_iterations>"<<endl;
 oss << pad << "   <link_staple_weight> " << linkStapleWeight 
     << " </link_staple_weight>"<<endl;
 oss << pad << "   <laph_sigma_cutoff> " << laphSigma 
     << " </laph_sigma_cutoff>"<<endl;
 oss << pad << "   <number_laph_eigvecs> " << laphNumEigvecs 
     << " </number_laph_eigvecs>"<<endl;
 oss << pad << "</stout_laph_smearing>"<<endl;
 return oss.str();
}


// *************************************************************
  }
}
