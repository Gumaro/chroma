//  $Id: sftmom.h,v 1.3 2003-03-20 19:34:25 flemingg Exp $
//  $Log: sftmom.h,v $
//  Revision 1.3  2003-03-20 19:34:25  flemingg
//  Evolved formfac_w.cc to use SftMom class, which included some bug fixes
//  in features in SftMom which had been previously untested and evolution
//  of the corresponding test program.
//
//  Revision 1.2  2003/03/14 17:13:44  flemingg
//  SftMom::sft() now works.
//
//  Revision 1.1  2003/03/14 05:06:06  flemingg
//  Initial version of SftMom class
//

#ifndef SFTMOM_INCLUDE
#define SFTMOM_INCLUDE

class SftMom
{
public:
  SftMom(int mom2_max, bool avg_equiv_mom=false, int j_decay=-1) ;

  SftMom(int mom2_max, multi1d<int> mom_offset,
         bool avg_equiv_mom=false, int j_decay=-1) 
  { init(mom2_max, mom_offset, avg_equiv_mom, j_decay) ; }

  const Set& getSubset() const { return sft_subsets ; }

  int numMom() { return num_mom ; }

  int numSubsets() const { return sft_subsets.numSubsets() ; }

  multi1d<int> numToMom(int mom_num) const { return mom_list[mom_num] ; }

  const LatticeComplex& operator[](int mom_num) const
    { return phases[mom_num] ; }

  multi2d<DComplex> sft(const LatticeComplex& cf) ;

private:
  SftMom() {} // hide default constructor

  void init(int mom2_max, multi1d<int> mom_offset,
            bool avg_equiv_mom=false, int j_decay=-1) ;

  multi2d<int> mom_list ;

  int num_mom ;

  multi1d<LatticeComplex> phases ;

  Set sft_subsets ;

} ;

#endif
